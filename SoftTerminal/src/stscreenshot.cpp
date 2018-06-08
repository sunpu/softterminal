#include "stscreenshot.h"

using namespace tahiti;

bool STScreenshot::isActivity = false;
bool SSScreen::isInit = false;

STScreenshot::STScreenshot(QWidget *parent) : QWidget(parent),
m_isLeftPressed(false), m_backgroundScreen(nullptr),
m_originPainting(nullptr), m_screen(nullptr)
{
	initGlobalScreen();
	initMeasureWidget();
	showFullScreen();
	setGeometry(getScreenRect());
	onEgoistic();
	setMouseTracking(true);
	Q_EMIT cursorPosChange(cursor().pos().x(), cursor().pos().y());
	updateMouse();
	show();
}

STScreenshot::~STScreenshot(void)
{
}

void STScreenshot::hideEvent(QHideEvent *)
{
	isActivity = false;
}


void STScreenshot::closeEvent(QCloseEvent *)
{
	isActivity = false;
}

void STScreenshot::mouseDoubleClickEvent(QMouseEvent *)
{
	Q_EMIT doubleClick();
}

void STScreenshot::initMeasureWidget(void)
{
	m_sizeShow.reset(new SSSizeShow(this));
	m_sizeShow->raise();
}

const QRect &STScreenshot::getScreenRect(void)
{
	if (!m_desktopRect.isEmpty())
	{
		return m_desktopRect;
	}
	m_desktopRect = QRect(QApplication::desktop()->pos(), QApplication::desktop()->size());
	return m_desktopRect;
}

std::shared_ptr<QPixmap> STScreenshot::initGlobalScreen(void)
{
	if (m_backgroundScreen.get() != nullptr)
	{
		return m_backgroundScreen;
	}

	std::shared_ptr<QPixmap> temp_screen = getGlobalScreen();
	QPixmap temp_dim_pix(temp_screen->width(), temp_screen->height());
	temp_dim_pix.fill((QColor(0, 0, 0, 160)));
	m_backgroundScreen.reset(new QPixmap(*temp_screen));
	QPainter p(m_backgroundScreen.get());
	p.drawPixmap(0, 0, temp_dim_pix);

	return m_backgroundScreen;
}

std::shared_ptr<QPixmap> STScreenshot::getGlobalScreen(void)
{
	if (m_originPainting.get() == nullptr)
	{
		QScreen *screen = QGuiApplication::primaryScreen();
		const QRect& temp_rect = getScreenRect();
		m_originPainting.reset(new QPixmap(screen->grabWindow(0, temp_rect.x(),
			temp_rect.y(), temp_rect.width(), temp_rect.height())));
	}
	return m_originPainting;
}

void STScreenshot::onEgoistic(void)
{
#ifdef Q_OS_WIN32
	SetWindowPos((HWND)this->winId(), HWND_TOPMOST, this->pos().x(), this->pos().y(), this->width(), this->height(), SWP_SHOWWINDOW);
#else
	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::WindowStaysOnTopHint;
	setWindowFlags(flags);
#endif
}

std::shared_ptr<SSScreen> STScreenshot::createScreen(const QPoint &pos)
{
	if (m_screen.get() == nullptr)
	{
		m_toolbar = new SSToolbar(this);

		m_screen.reset(new SSScreen(m_originPainting, pos, m_toolbar, this));
		connect(this, SIGNAL(cursorPosChange(int, int)), m_screen.get(), SLOT(onMouseChange(int, int)));
		connect(this, SIGNAL(doubleClick()), m_screen.get(), SLOT(onSaveScreen()));
		connect(m_screen.get(), SIGNAL(finishScreenshot(QString)), this, SLOT(onFinishScreenshot(QString)));
		connect(m_screen.get(), SIGNAL(cancelScreenshot()), this, SLOT(onCancelScreenshot()));

		connect(m_screen.get(), SIGNAL(sizeChange(int, int)), m_sizeShow.get(), SLOT(onSizeChange(int, int)));
		connect(m_screen.get(), SIGNAL(postionChange(int, int)), m_sizeShow.get(), SLOT(onPostionChange(int, int)));

		connect(m_toolbar, SIGNAL(finishScreenshot()), m_screen.get(), SLOT(onSaveScreen()));
		connect(m_toolbar, SIGNAL(cancelScreenshot()), m_screen.get(), SLOT(quitScreenshot()));
		connect(m_screen.get(), SIGNAL(posChange(int, int)), m_toolbar, SLOT(onPosChange(int, int)));

		m_startPoint = pos;
		m_isLeftPressed = true;
	}
	return m_screen;
}

void STScreenshot::destroyScreen()
{
	if (m_screen.get() != nullptr)
	{
		disconnect(this, SIGNAL(doubleClick()), m_screen.get(), SLOT(onSaveScreen()));
		disconnect(m_screen.get(), SIGNAL(sizeChange(int, int)), m_sizeShow.get(), SLOT(onSizeChange(int, int)));
		disconnect(m_screen.get(), SIGNAL(postionChange(int, int)), m_sizeShow.get(), SLOT(onPostionChange(int, int)));
		/// 清理工具
		m_screen.reset();
		m_screen = nullptr;
		m_isLeftPressed = false;
		update();
		return;
	}
}

void STScreenshot::onFinishScreenshot(QString path)
{
	Q_EMIT finishScreenshot(path);
}

void STScreenshot::onCancelScreenshot()
{
	Q_EMIT cancelScreenshot();
}

void STScreenshot::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton)
	{
		createScreen(e->pos());
		return;
	}
}

void STScreenshot::mouseReleaseEvent(QMouseEvent *e)
{
	if (e->button() == Qt::RightButton)
	{
		if (m_screen.get() != nullptr)
		{
			m_sizeShow->hide();
			/// 更新当前鼠标选中的窗口
			updateMouse();
			return destroyScreen();
		}
		close();
		return;
	}
	else if (m_isLeftPressed == true && e->button() == Qt::LeftButton)
	{
		m_toolbar->show();
		if (m_startPoint == e->pos() && !m_windowRect.isEmpty())
		{
			m_screen->setGeometry(m_windowRect);
			m_screen->show();
			m_windowRect = {};
		}
		disconnect(this, SIGNAL(cursorPosChange(int, int)), m_screen.get(), SLOT(onMouseChange(int, int)));
		m_isLeftPressed = false;
	}
	QWidget::mouseReleaseEvent(e);
}

void STScreenshot::mouseMoveEvent(QMouseEvent *e)
{
	Q_EMIT cursorPosChange(e->x(), e->y());
	if (m_isLeftPressed)
	{
		m_windowRect = {};
		update();
	}
	else if (m_isLeftPressed == false && false == m_screen->state())
	{
		onEgoistic();
		updateMouse();
	}
	QWidget::mouseMoveEvent(e);
}

void STScreenshot::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.drawPixmap(0, 0, m_desktopRect.width(), m_desktopRect.height(), *m_backgroundScreen);

	if (!m_windowRect.isEmpty())
	{
		QPen pen = painter.pen();
		pen.setColor(QColor("#30ba78"));
		pen.setWidth(3);
		painter.setPen(pen);
		painter.drawRect(m_windowRect.x(), m_windowRect.y(), m_windowRect.width(), m_windowRect.height());
		painter.drawPixmap(QPoint(m_windowRect.x(), m_windowRect.y()), *m_originPainting, m_windowRect);
	}
}

void STScreenshot::updateMouse(void)
{
	::EnableWindow((HWND)winId(), FALSE);
	/// @marker: 只更新一次,可以修复用户误操作导致的查找窗口与识别界面窗口不一致.
	//OECommonHelper::getSmallestWindowFromCursor(m_windowRect);
	QPoint temp_pt = mapFromGlobal(QPoint(m_windowRect.x(), m_windowRect.y()));
	m_windowRect = QRect(temp_pt.x(), temp_pt.y(), m_windowRect.width(), m_windowRect.height());
	::EnableWindow((HWND)winId(), TRUE);
	Q_EMIT findChildWind(m_windowRect);
	update();
}

void STScreenshot::keyPressEvent(QKeyEvent *e)
{
	/// Esc 键退出截图;
	if (e->key() == Qt::Key_Escape)
	{
		Q_EMIT cancelScreenshot();
		close();
	}
	else
	{
		e->ignore();
	}

}

// ------------------------ SSSizeShow ------------------------
SSSizeShow::SSSizeShow(QWidget *parent) : QWidget(parent)
{

	/// 设置感知器默认大小
	/*setFixedSize(95 * OECommonHelper::getWindowHeightMultiplyingPower(),
		20 * OECommonHelper::getWindowHeightMultiplyingPower());*/
	resize(QSize(80, 30));
	setStyleSheet("border-radius: 3px;font: 10pt \"微软雅黑\";");

	hide();
}

void SSSizeShow::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setBrush(QColor(0, 0, 0));
	painter.drawRoundRect(rect(), 10, 10);
	//painter.drawPixmap(rect(), *m_backgroundPixmap);
	painter.setPen(QPen(QColor(Qt::white)));
	painter.drawText(rect(), Qt::AlignCenter, m_info);
}

void SSSizeShow::onPostionChange(int x, int y)
{
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	const int& ry = y - height() - 1;
	if (ry < 0)
	{
		this->raise();
	}
	move(x, ((ry < 0) ? y : ry));
	show();
}

void SSSizeShow::onSizeChange(int w, int h)
{
	m_info = QString("%1 * %2").arg(w).arg(h);
}

// ------------------------ SSScreen ------------------------
SSScreen::SSScreen(std::shared_ptr<QPixmap> originPainting, QPoint pos, SSToolbar* toolbar, QWidget *parent)
	: QWidget(parent), m_direction(NONE), m_originPoint(pos), m_toolbar(toolbar),
	m_isPressed(false), m_originPainting(originPainting)
{
	connect(this, SIGNAL(doubleClick()), this, SLOT(onSaveScreen()));

	setMouseTracking(true);
	hide();
}

DIRECTION SSScreen::getRegion(const QPoint &cursor)
{
	if (!isInit)
	{
		return NONE;
	}
	DIRECTION ret_dir = NONE;
	// left upper
	QPoint pt_lu = mapToParent(rect().topLeft());
	// right lower
	QPoint pt_rl = mapToParent(rect().bottomRight());

	int x = cursor.x();
	int y = cursor.y();

	if (pt_lu.x() + PADDING >= x
		&& pt_lu.x() <= x
		&& pt_lu.y() + PADDING >= y
		&& pt_lu.y() <= y)
	{
		// 左上角
		ret_dir = LEFTUPPER;
		this->setCursor(QCursor(Qt::SizeFDiagCursor));
	}
	else if (x >= pt_rl.x() - PADDING
		&& x <= pt_rl.x()
		&& y >= pt_rl.y() - PADDING
		&& y <= pt_rl.y())
	{
		// 右下角
		ret_dir = RIGHTLOWER;
		this->setCursor(QCursor(Qt::SizeFDiagCursor));
	}
	else if (x <= pt_lu.x() + PADDING
		&& x >= pt_lu.x()
		&& y >= pt_rl.y() - PADDING
		&& y <= pt_rl.y())
	{
		// 左下角
		ret_dir = LEFTLOWER;
		this->setCursor(QCursor(Qt::SizeBDiagCursor));
	}
	else if (x <= pt_rl.x()
		&& x >= pt_rl.x() - PADDING
		&& y >= pt_lu.y()
		&& y <= pt_lu.y() + PADDING)
	{
		// 右上角
		ret_dir = RIGHTUPPER;
		this->setCursor(QCursor(Qt::SizeBDiagCursor));
	}
	else if (x <= pt_lu.x() + PADDING
		&& x >= pt_lu.x())
	{
		// 左边
		ret_dir = LEFT;
		this->setCursor(QCursor(Qt::SizeHorCursor));
	}
	else if (x <= pt_rl.x()
		&& x >= pt_rl.x() - PADDING)
	{
		// 右边
		ret_dir = RIGHT;
		this->setCursor(QCursor(Qt::SizeHorCursor));
	}
	else if (y >= pt_lu.y()
		&& y <= pt_lu.y() + PADDING)
	{
		// 上边
		ret_dir = UPPER;
		this->setCursor(QCursor(Qt::SizeVerCursor));
	}
	else if (y <= pt_rl.y()
		&& y >= pt_rl.y() - PADDING)
	{
		// 下边
		ret_dir = LOWER;
		this->setCursor(QCursor(Qt::SizeVerCursor));
	}
	else
	{
		// 默认
		ret_dir = NONE;
		this->setCursor(QCursor(Qt::SizeAllCursor));
	}
	return ret_dir;
}

void SSScreen::contextMenuEvent(QContextMenuEvent *)
{
}

void SSScreen::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton)
	{
		Q_EMIT doubleClick();
		e->accept();
	}
}

void SSScreen::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton)
	{
		m_isPressed = true;
		if (m_direction != NONE)
		{
			this->mouseGrabber();
		}
		m_movePos = e->globalPos() - pos();

		m_toolbar->hide();
	}
}

void SSScreen::mouseReleaseEvent(QMouseEvent * e)
{
	if (e->button() == Qt::LeftButton)
	{
		m_isPressed = false;
		if (m_direction != NONE)
		{
			setCursor(QCursor(Qt::SizeAllCursor));
		}

		m_toolbar->show();
	}
}

void SSScreen::mouseMoveEvent(QMouseEvent * e)
{
	QPoint gloPoint = mapToParent(e->pos());
	// left upper
	QPoint pt_lu = mapToParent(rect().topLeft());
	// left lower
	QPoint pt_ll = mapToParent(rect().bottomLeft());
	// right lower
	QPoint pt_rl = mapToParent(rect().bottomRight());
	// right upper
	QPoint pt_ru = mapToParent(rect().topRight());
	if (!m_isPressed)
	{
		m_direction = getRegion(gloPoint);

		switch (m_direction)
		{
		case NONE:
		case RIGHT:
		case RIGHTLOWER:
			m_originPoint = pt_lu;
			break;
		case RIGHTUPPER:
			m_originPoint = pt_ll;
			break;
		case LEFT:
		case LEFTLOWER:
			m_originPoint = pt_ru;
			break;
		case LEFTUPPER:
		case UPPER:
			m_originPoint = pt_rl;
			break;
		case LOWER:
			m_originPoint = pt_lu;
			break;
		}
	}
	else
	{
		if (m_direction != NONE)
		{
			const int& global_x = gloPoint.x();
			switch (m_direction)
			{
			case LEFT:
				return onMouseChange(global_x, pt_ll.y() + 1);
			case RIGHT:
				return onMouseChange(global_x, pt_rl.y() + 1);
			case UPPER:
				return onMouseChange(pt_lu.x(), gloPoint.y());
			case LOWER:
				return onMouseChange(pt_rl.x() + 1, gloPoint.y());
			case LEFTUPPER:
			case RIGHTUPPER:
			case LEFTLOWER:
			case RIGHTLOWER:
				return onMouseChange(global_x, gloPoint.y());
			default:
				break;
			}
		}
		else
		{
			move(e->globalPos() - m_movePos);
			m_movePos = e->globalPos() - pos();
		}
	}
	m_currentRect = geometry();
}

void SSScreen::moveEvent(QMoveEvent *)
{
	Q_EMIT postionChange(x(), y());
	Q_EMIT posChange(x() + width(), y() + height());
}

void SSScreen::resizeEvent(QResizeEvent *)
{
	Q_EMIT sizeChange(width(), height());
	Q_EMIT posChange(x() + width(), y() + height());
}

void SSScreen::showEvent(QShowEvent *)
{
	isInit = true;
}

void SSScreen::hideEvent(QHideEvent *)
{
	m_currentRect = {};
	m_movePos = {};
	m_originPoint = {};
	isInit = false;
}

void SSScreen::enterEvent(QEvent *e)
{
	setCursor(Qt::SizeAllCursor);
	QWidget::enterEvent(e);
}

void SSScreen::leaveEvent(QEvent *e)
{
	setCursor(Qt::ArrowCursor);
	QWidget::leaveEvent(e);
}

void SSScreen::closeEvent(QCloseEvent *)
{
	isInit = false;
}

void SSScreen::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.drawPixmap(QPoint(0, 0), *m_originPainting, m_currentRect);

	QPen pen(QColor("#30ba78"), 5);
	painter.setPen(pen);
	painter.drawRect(rect());
}

const QString SSScreen::getFileName(void)
{
	QString file_name = QDateTime::currentDateTime().
		toString(QStringLiteral("CSDN博客_瓯裔_yyyy-MM-dd-HH-mm-ss"));
	return file_name;
}

void SSScreen::onSaveScreenOther(void)
{
	QString fileName = QFileDialog::getSaveFileName(this,
		QStringLiteral("保存图片"),
		getFileName(), "JPEG Files (*.jpg)");

	if (fileName.length() > 0)
	{
		m_originPainting->copy(m_currentRect).save(fileName, "jpg");
		quitScreenshot();
	}
}

QString SSScreen::getRandString()
{
	int max = 16;
	QString tmp = QString("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
	QString str = QString();
	QTime t;
	t = QTime::currentTime();
	qsrand(t.msec() + t.second() * 1000);
	for (int i = 0; i < max; i++)
	{
		int ir = qrand() % tmp.length();
		str[i] = tmp.at(ir);
	}
	return str;
}

void SSScreen::onSaveScreen(void)
{
	QString randStr = getRandString();
	QString fileName = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
		+ DATA_ROOT_PATH + TMP_PATH + randStr + ".png";
	m_originPainting->copy(m_currentRect).save(fileName, "png");
	Q_EMIT finishScreenshot(fileName);
	quitScreenshot();
}


void SSScreen::quitScreenshot(void)
{
	Q_EMIT cancelScreenshot();
	close();
	parentWidget()->close();
}

void SSScreen::onMouseChange(int x, int y)
{
	show();
	if (x < 0 || y < 0)
	{
		return;
	}
	const int& rx = (x >= m_originPoint.x()) ? m_originPoint.x() : x;
	const int& ry = (y >= m_originPoint.y()) ? m_originPoint.y() : y;
	const int& rw = abs(x - m_originPoint.x());
	const int& rh = abs(y - m_originPoint.y());

	m_currentRect = QRect(rx, ry, rw, rh);

	this->setGeometry(m_currentRect);
	parentWidget()->update();
}

// ------------------------ SSToolbar ------------------------
SSToolbar::SSToolbar(QWidget *parent) :
	QWidget(parent),
	screen(NULL)
{
	ui.setupUi(this);
}

SSToolbar::~SSToolbar()
{
}

void SSToolbar::on_pbCancel_clicked()
{
	Q_EMIT cancelScreenshot();
}

void SSToolbar::on_pbFinish_clicked()
{
	Q_EMIT finishScreenshot();
}

void SSToolbar::onPosChange(int x, int y)
{
	setGeometry(x - 130, y + 3, 130, 30);
}
