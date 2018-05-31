#include "stloginrotate.h"

using namespace tahiti;

/* 登录窗口 */
STLogin::STLogin(XmppClient* client) : m_xmppClient(client)
{
	ui.setupUi(this);
	initLoginData();

	connect(m_xmppClient, SIGNAL(loginResult(bool)), this, SLOT(handleLoginResult(bool)));

	ui.leUserName->installEventFilter(this);
	ui.lePasswd->installEventFilter(this);

	m_load = new STLoad(this);
	m_load->move(QPoint((width() - 120) / 2, (height() - 120) / 2));
	m_load->hide();

	bool autoLogin = (STConfig::getConfig("/config/autoLogin") == "true") ? true : false;
	if (autoLogin)
	{
		on_pbLogin_clicked();
	}
}

STLogin::~STLogin()
{

}

void STLogin::initLoginData()
{
	// 读取数据
	bool rememberPasswd = (STConfig::getConfig("/config/rememberPasswd") == "true") ? true : false;
	ui.cbRemeberPasswd->setChecked(rememberPasswd);
	bool autoLogin = (STConfig::getConfig("/config/autoLogin") == "true") ? true : false;
	ui.cbAutoLogin->setChecked(autoLogin);

	m_currentUser = STConfig::getConfig("/xmpp/user");
	ui.leUserName->setText(STConfig::getConfig("/xmpp/user"));
	if (rememberPasswd)
	{
		ui.lePasswd->setText(STConfig::getConfig("/xmpp/passwd"));
	}
	else
	{
		ui.lePasswd->setText("");
	}

	QString userName = STConfig::getConfig("/xmpp/user");
	QString path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
		+ DATA_ROOT_PATH + AVATAR_PATH;
	QString picFilePath;
	QDir dir;
	dir.setPath(path);
	QDirIterator iter(dir, QDirIterator::Subdirectories);
	while (iter.hasNext())
	{
		iter.next();
		QFileInfo info = iter.fileInfo();
		if (info.isFile() && info.fileName().split("@")[0] == userName)
		{
			picFilePath = path + info.fileName();
			break;
		}
	}
	if (picFilePath.size() == 0)
	{
		picFilePath = ":/SoftTerminal/images/account.png";
	}
	QImage* image = new QImage(picFilePath);
	ui.lblUserPic->setPixmap(QPixmap::fromImage(*image).scaled(80, 80));

	if (ui.leUserName->text().size() == 0)
	{
		ui.leUserName->setFocus();
	}
	else
	{
		ui.lePasswd->setFocus();
	}
}

void STLogin::onUserNameChanged()
{
	if (ui.leUserName->text() == m_currentUser &&
		STConfig::getConfig("/config/rememberPasswd") == "true")
	{
		ui.lePasswd->setText(STConfig::getConfig("/xmpp/passwd"));
	}
	else
	{
		ui.lePasswd->clear();
	}

	QString path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
		+ DATA_ROOT_PATH + AVATAR_PATH;
	QString picFilePath;
	QDir dir;
	dir.setPath(path);
	QDirIterator iter(dir, QDirIterator::Subdirectories);
	while (iter.hasNext())
	{
		iter.next();
		QFileInfo info = iter.fileInfo();
		if (info.isFile() && info.fileName().split("@")[0] == ui.leUserName->text())
		{
			picFilePath = path + info.fileName();
			break;
		}
	}
	if (picFilePath.size() == 0)
	{
		picFilePath = ":/SoftTerminal/images/account.png";
	}
	QImage* image = new QImage(picFilePath);
	ui.lblUserPic->setPixmap(QPixmap::fromImage(*image).scaled(80, 80));
	
}

void STLogin::on_pbLogin_clicked()
{
	QString server = STConfig::getConfig("/xmpp/server");
	QString port = STConfig::getConfig("/xmpp/port");
	if (server.size() == 0 ||+ port.size() == 0)
	{
		QPoint pos(ui.pbLogin->mapToGlobal(ui.pbLogin->pos()).x() + ui.pbLogin->width() / 2,
			ui.pbLogin->mapToGlobal(ui.pbLogin->pos()).y());
		QToolTip::showText(pos, QStringLiteral("未配置服务器地址"), ui.pbLogin);
		return;
	}

	QString user = ui.leUserName->text();
	QString passwd = ui.lePasswd->text();
	if (user.size() == 0)
	{
		ui.leUserName->setFocus();
		QPoint pos(ui.leUserName->mapToGlobal(ui.leUserName->pos()).x() - ui.leUserName->width() / 2,
			ui.leUserName->mapToGlobal(ui.leUserName->pos()).y());
		QToolTip::showText(pos, QStringLiteral("未输入账号"), ui.leUserName);
		return;
	}
	if (passwd.size() == 0)
	{
		ui.lePasswd->setFocus();
		QPoint pos(ui.lePasswd->mapToGlobal(ui.lePasswd->pos()).x() - ui.lePasswd->width() / 2,
			ui.lePasswd->mapToGlobal(ui.lePasswd->pos()).y());
		QToolTip::showText(pos, QStringLiteral("未输入密码"), ui.lePasswd);
		return;
	}
	m_xmppClient->setXmppAccount(user, passwd, server, port);
	m_xmppClient->run();
	setLoadStatus(true);
}

void STLogin::handleLoginResult(bool result)
{
	if (result)
	{
		// 保存数据
		bool rememberPasswd = ui.cbRemeberPasswd->isChecked();
		STConfig::setConfig("/config/rememberPasswd", rememberPasswd ? "true" : "false");
		bool autoLogin = ui.cbAutoLogin->isChecked();
		STConfig::setConfig("/config/autoLogin", autoLogin ? "true" : "false");

		QString user = ui.leUserName->text();
		STConfig::setConfig("/xmpp/user", user);
		if (rememberPasswd)
		{
			STConfig::setConfig("/xmpp/passwd", ui.lePasswd->text());
		}
		else
		{
			STConfig::setConfig("/xmpp/passwd", "");
		}

		// 创建个人聊天记录目录
		QString path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
			+ DATA_ROOT_PATH + RECORD_PATH + user;
		QDir* dir = new QDir;
		if (!dir->exists(path))
		{
			dir->mkpath(path);
		}

		// 查询用户花名册
		m_xmppClient->queryRoster();

		pthread_create(&m_tidLoad, NULL, loadProc, this);
	}
	else
	{
		setLoadStatus(false);
		ui.leUserName->setFocus();
		QPoint pos(ui.leUserName->mapToGlobal(ui.leUserName->pos()).x() - ui.leUserName->width() / 2,
			ui.leUserName->mapToGlobal(ui.leUserName->pos()).y());
		QToolTip::showText(pos, QStringLiteral("账号或密码错误"), ui.leUserName);
	}
}

void STLogin::setLoadStatus(bool status)
{
	if (status)
	{
		m_load->show();
		ui.pbLogin->setText(QStringLiteral("登　录　中"));
		ui.pbLogin->setStyleSheet("QPushButton{border-radius:3px;color:rgb(255, 255, 255);background-color:rgb(145, 146, 155);}");
	}
	else
	{
		m_load->hide();
		ui.pbLogin->setText(QStringLiteral("登　录"));
		ui.pbLogin->setStyleSheet("QPushButton{border-radius:3px;color:rgb(255, 255, 255);"
			"background-color:rgb(48, 186, 120);}QPushButton:hover:pressed{background-color:rgb(48, 186, 120);}"
			"QPushButton:hover:!pressed{background-color:rgb(109, 206, 160);}");
	}
	ui.cbAutoLogin->setEnabled(!status);
	ui.cbRemeberPasswd->setEnabled(!status);
	ui.leUserName->setEnabled(!status);
	ui.lePasswd->setEnabled(!status);
	ui.pbLogin->setEnabled(!status);
	ui.pb2Regist->setEnabled(!status);
	ui.pb2Config->setEnabled(!status);
}

void* STLogin::loadProc(void* args)
{
	STLogin* login = (STLogin*)args;

	QThread::sleep(3);
	// 窗口切换
	login->changeMainWindow();
	return NULL;
}

void STLogin::on_pb2Regist_clicked()
{
	Q_EMIT rotateWindow(1);
}

void STLogin::on_pb2Config_clicked()
{
	Q_EMIT rotateWindow(2);
}

void STLogin::on_pbMinimum_clicked()
{
	this->parentWidget()->showMinimized();
}

void STLogin::on_pbClose_clicked()
{
	this->parentWidget()->close();
}

void STLogin::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}

	return QWidget::mousePressEvent(event);
}

void STLogin::mouseMoveEvent(QMouseEvent *event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint widgetPos = this->parentWidget()->pos() + movePoint;
		m_startMovePos = event->globalPos();
		this->parentWidget()->move(widgetPos.x(), widgetPos.y());
	}
	return QWidget::mouseMoveEvent(event);
}

void STLogin::mouseReleaseEvent(QMouseEvent *event)
{
	m_isPressed = false;
	return QWidget::mouseReleaseEvent(event);
}

bool STLogin::eventFilter(QObject *obj, QEvent *e)
{
	if (e->type() == QEvent::KeyPress
		&& (ui.leUserName == obj || ui.lePasswd == obj))
	{
		QKeyEvent *event = static_cast<QKeyEvent*>(e);
		if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
		{
			on_pbLogin_clicked();
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/* 注册窗口 */
STRegist::STRegist(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);
	ui.leUserName->installEventFilter(this);
	ui.lePasswd->installEventFilter(this);
	ui.lePasswdVerify->installEventFilter(this);
}

STRegist::~STRegist()
{

}

void STRegist::initRegistData()
{
	ui.leUserName->setText("");
	ui.lePasswd->setText("");
	ui.lePasswdVerify->setText("");
	ui.leUserName->setFocus();
}

void STRegist::handleRegistResult(bool result)
{
	if (result)
	{
		// 保存数据
		STConfig::setConfig("/config/rememberPasswd", "false");
		STConfig::setConfig("/config/autoLogin", "false");

		STConfig::setConfig("/xmpp/user", ui.leUserName->text());
		STConfig::setConfig("/xmpp/passwd", ui.lePasswd->text());

		// 切换到登录窗口
		Q_EMIT rotateWindow(0);
	}
	else
	{
		ui.leUserName->setFocus();
		QPoint pos(ui.leUserName->mapToGlobal(ui.leUserName->pos()).x() - ui.leUserName->width() / 2,
			ui.leUserName->mapToGlobal(ui.leUserName->pos()).y());
		QToolTip::showText(pos, QStringLiteral("注册失败"), ui.leUserName);
	}
}

void STRegist::on_pb2Login_clicked()
{
	Q_EMIT rotateWindow(0);
}

void STRegist::on_pbRegist_clicked()
{
	QString user = ui.leUserName->text();
	QString passwd = ui.lePasswd->text();
	QString passwdVerify = ui.lePasswdVerify->text();
	QString server = STConfig::getConfig("/xmpp/server");
	QString port = STConfig::getConfig("/xmpp/port");
	if (server.size() == 0 || +port.size() == 0)
	{
		QPoint pos(ui.pbRegist->mapToGlobal(ui.pbRegist->pos()).x() + ui.pbRegist->width() / 2,
			ui.pbRegist->mapToGlobal(ui.pbRegist->pos()).y());
		QToolTip::showText(pos, QStringLiteral("未配置服务器地址"), ui.pbRegist);
		return;
	}
	m_xmppRegister = new XmppRegister(server, port);
	connect((const QObject *)m_xmppRegister, SIGNAL(registResult(bool)), this, SLOT(handleRegistResult(bool)));

	if (user.size() == 0)
	{
		ui.leUserName->setFocus();
		QPoint pos(ui.leUserName->mapToGlobal(ui.leUserName->pos()).x() - ui.leUserName->width() / 2,
			ui.leUserName->mapToGlobal(ui.leUserName->pos()).y());
		QToolTip::showText(pos, QStringLiteral("未输入账号"), ui.leUserName);
		return;
	}

	if (passwd.size() == 0)
	{
		ui.lePasswd->setFocus();
		QPoint pos(ui.lePasswd->mapToGlobal(ui.lePasswd->pos()).x() - ui.lePasswd->width() / 2,
			ui.lePasswd->mapToGlobal(ui.lePasswd->pos()).y());
		QToolTip::showText(pos, QStringLiteral("未输入密码"), ui.lePasswd);
		return;
	}

	if (passwdVerify.size() == 0)
	{
		ui.lePasswdVerify->setFocus();
		QPoint pos(ui.lePasswdVerify->mapToGlobal(ui.lePasswdVerify->pos()).x() - ui.lePasswdVerify->width() / 2,
			ui.lePasswdVerify->mapToGlobal(ui.lePasswdVerify->pos()).y());
		QToolTip::showText(pos, QStringLiteral("未输入确认密码"), ui.lePasswdVerify);
		return;
	}

	if (passwd != passwdVerify)
	{
		ui.lePasswdVerify->setFocus();
		QPoint pos(ui.lePasswdVerify->mapToGlobal(ui.lePasswdVerify->pos()).x() - ui.lePasswdVerify->width() / 2,
			ui.lePasswdVerify->mapToGlobal(ui.lePasswdVerify->pos()).y());
		QToolTip::showText(pos, QStringLiteral("输入密码不一致"), ui.lePasswdVerify);
		return;
	}

	m_xmppRegister->registAccount(user, passwd);
}

void STRegist::on_pbMinimum_clicked()
{
	this->parentWidget()->showMinimized();
}

void STRegist::on_pbClose_clicked()
{
	this->parentWidget()->close();
}

void STRegist::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}

	return QWidget::mousePressEvent(event);
}

void STRegist::mouseMoveEvent(QMouseEvent *event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint widgetPos = this->parentWidget()->pos() + movePoint;
		m_startMovePos = event->globalPos();
		this->parentWidget()->move(widgetPos.x(), widgetPos.y());
	}
	return QWidget::mouseMoveEvent(event);
}

void STRegist::mouseReleaseEvent(QMouseEvent *event)
{
	m_isPressed = false;
	return QWidget::mouseReleaseEvent(event);
}

bool STRegist::eventFilter(QObject *obj, QEvent *e)
{
	if (e->type() == QEvent::KeyPress
		&& (ui.leUserName == obj || ui.lePasswd == obj || ui.lePasswdVerify == obj))
	{
		QKeyEvent *event = static_cast<QKeyEvent*>(e);
		if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
		{
			on_pbRegist_clicked();
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/* 服务器配置窗口 */
STServerConfig::STServerConfig(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);
	ui.leServerIP->installEventFilter(this);
}

STServerConfig::~STServerConfig()
{

}

void STServerConfig::initServerConfigData()
{
	QString server = STConfig::getConfig("/xmpp/server");
	ui.leServerIP->setText(server);
	QString port = STConfig::getConfig("/xmpp/port");
	if (port.size() == 0)
	{
		port = "5222";
		STConfig::setConfig("/xmpp/port", port);
	}
	ui.leServerPort->setText(port);
	ui.leServerIP->setFocus();
}

void STServerConfig::on_pb2Login_clicked()
{
	Q_EMIT rotateWindow(0);
}

void STServerConfig::on_pbConfirm_clicked()
{
	STConfig::setConfig("/xmpp/server", ui.leServerIP->text());
	STConfig::setConfig("/xmpp/port", ui.leServerPort->text());

	if (ui.leServerIP->text().size() == 0)
	{
		ui.leServerIP->setFocus();
		QPoint pos(ui.leServerIP->mapToGlobal(ui.leServerIP->pos()).x() - ui.leServerIP->width() / 2,
			ui.leServerIP->mapToGlobal(ui.leServerIP->pos()).y());
		QToolTip::showText(pos, QStringLiteral("未输入服务器地址"), ui.leServerIP);
		return;
	}

	if (ui.leServerPort->text().size() == 0)
	{
		ui.leServerPort->setFocus();
		QPoint pos(ui.leServerPort->mapToGlobal(ui.leServerPort->pos()).x() - ui.leServerPort->width() / 2,
			ui.leServerPort->mapToGlobal(ui.leServerPort->pos()).y());
		QToolTip::showText(pos, QStringLiteral("未输入服务器端口"), ui.leServerPort);
		return;
	}

	// 切换到登录窗口
	Q_EMIT rotateWindow(0);
}

void STServerConfig::on_pbMinimum_clicked()
{
	this->parentWidget()->showMinimized();
}

void STServerConfig::on_pbClose_clicked()
{
	this->parentWidget()->close();
}

void STServerConfig::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}

	return QWidget::mousePressEvent(event);
}

void STServerConfig::mouseMoveEvent(QMouseEvent *event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint widgetPos = this->parentWidget()->pos() + movePoint;
		m_startMovePos = event->globalPos();
		this->parentWidget()->move(widgetPos.x(), widgetPos.y());
	}
	return QWidget::mouseMoveEvent(event);
}

void STServerConfig::mouseReleaseEvent(QMouseEvent *event)
{
	m_isPressed = false;
	return QWidget::mouseReleaseEvent(event);
}

bool STServerConfig::eventFilter(QObject *obj, QEvent *e)
{
	if (e->type() == QEvent::KeyPress && ui.leServerIP == obj)
	{
		QKeyEvent *event = static_cast<QKeyEvent*>(e);
		if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
		{
			on_pbConfirm_clicked();
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/* 旋转窗口 */
STLoginRotate::STLoginRotate(XmppClient* client)
	: m_isRoratingWindow(false)
	, m_nextPageIndex(0)
	, m_xmppClient(client)
{
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowMinimizeButtonHint);
	this->setAttribute(Qt::WA_TranslucentBackground);
	this->setWindowTitle(QStringLiteral("SoftTerminal VMediaX"));
	QIcon icon;
	icon.addFile(QStringLiteral(":/SoftTerminal/images/logo.png"), QSize(), QIcon::Normal, QIcon::Off);
	this->setWindowIcon(icon);

	// 给窗口设置rotateValue属性;
	this->setProperty("rotateValue", 0);

	initRotateWindow();
}

STLoginRotate::~STLoginRotate()
{

}

// 初始化旋转的窗口;
void STLoginRotate::initRotateWindow()
{
	// 这里定义了两个信号，需要自己去发送信号;
	m_loginWindow = new STLogin(m_xmppClient);
	connect(m_loginWindow, SIGNAL(rotateWindow(int)), this, SLOT(onRotateWindow(int)));
	connect(m_loginWindow, SIGNAL(changeMainWindow()), this, SLOT(onChangeMainWindow()));
	m_registWindow = new STRegist();
	connect(m_registWindow, SIGNAL(rotateWindow(int)), this, SLOT(onRotateWindow(int)));
	m_serverConfigWindow = new STServerConfig();
	connect(m_serverConfigWindow, SIGNAL(rotateWindow(int)), this, SLOT(onRotateWindow(int)));

	this->addWidget(m_loginWindow);
	this->addWidget(m_registWindow);
	this->addWidget(m_serverConfigWindow);

	// 这里宽和高都增加，是因为在旋转过程中窗口宽和高都会变化;
	this->setFixedSize(QSize(m_loginWindow->width() + 20, m_loginWindow->height() + 100));

	onRotateFinished();
}

// 开始旋转窗口;
void STLoginRotate::onRotateWindow(int index)
{
	// 如果窗口正在旋转，直接返回;
	if (m_isRoratingWindow)
	{
		return;
	}
	m_isRoratingWindow = true;
	m_nextPageIndex = index;
	QPropertyAnimation *rotateAnimation = new QPropertyAnimation(this, "rotateValue");
	// 设置旋转持续时间;
	rotateAnimation->setDuration(600);
	// 设置旋转角度变化趋势;
	rotateAnimation->setEasingCurve(QEasingCurve::InCubic);
	// 设置旋转角度范围;
	rotateAnimation->setStartValue(0);
	rotateAnimation->setEndValue(180);
	connect(rotateAnimation, SIGNAL(valueChanged(QVariant)), this, SLOT(repaint()));
	connect(rotateAnimation, SIGNAL(finished()), this, SLOT(onRotateFinished()));
	// 隐藏当前窗口，通过不同角度的绘制来达到旋转的效果;
	currentWidget()->hide();
	rotateAnimation->start();
}

// 旋转结束;
void STLoginRotate::onRotateFinished()
{
	m_isRoratingWindow = false;
	setCurrentWidget(widget(m_nextPageIndex));
	repaint();
	if (m_nextPageIndex == 0)
	{
		m_loginWindow->initLoginData();
	}
	else if (m_nextPageIndex == 1)
	{
		m_registWindow->initRegistData();
	}
	else
	{
		m_serverConfigWindow->initServerConfigData();
	}
}

// 绘制旋转效果;
void STLoginRotate::paintEvent(QPaintEvent* event)
{
	if (m_isRoratingWindow)
	{
		// 小于90度时;
		int rotateValue = this->property("rotateValue").toInt();
		if (rotateValue <= 90)
		{
			QPixmap rotatePixmap(currentWidget()->size());
			currentWidget()->render(&rotatePixmap);
			QPainter painter(this);
			painter.setRenderHint(QPainter::Antialiasing);
			QTransform transform;
			transform.translate(width() / 2, 0);
			transform.rotate(rotateValue, Qt::YAxis);
			painter.setTransform(transform);
			painter.drawPixmap(-1 * width() / 2, 0, rotatePixmap);
		}
		// 大于90度时
		else
		{
			QPixmap rotatePixmap(widget(m_nextPageIndex)->size());
			widget(m_nextPageIndex)->render(&rotatePixmap);
			QPainter painter(this);
			painter.setRenderHint(QPainter::Antialiasing);
			QTransform transform;
			transform.translate(width() / 2, 0);
			transform.rotate(rotateValue + 180, Qt::YAxis);
			painter.setTransform(transform);
			painter.drawPixmap(-1 * width() / 2, 0, rotatePixmap);
		}
	}
	else
	{
		return __super::paintEvent(event);
	}
}

void STLoginRotate::onChangeMainWindow()
{
	m_mainWindow = new STMain(m_xmppClient);
	connect(m_mainWindow, SIGNAL(changeLoginWindow()), this, SLOT(onChangeLoginWindow()));
	m_mainWindow->init();
	this->hide();
	m_mainWindow->show();
}

void STLoginRotate::onChangeLoginWindow()
{
	m_mainWindow->hide();
	m_mainWindow->destroy();
	m_mainWindow = NULL;
	m_loginWindow->setLoadStatus(false);
	m_xmppClient->logout();
	onRotateFinished();
	this->show();
}
