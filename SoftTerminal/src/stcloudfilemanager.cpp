#include "stcloudfilemanager.h"

using namespace tahiti;

STCloudFileManager::STCloudFileManager(QWidget * parent) : QWidget(parent)
{
	ui.setupUi(this);
	defaultBkColor = QColor(255, 255, 255);

	m_support = new STCloudSupport(this);
	m_support->hide();

	previousColorRow = -1;
	ui.twFileManager->setMouseTracking(true);
	//connect(ui.twFileManager, SIGNAL(cellEntered(int, int)), this, SLOT(changeRowColor(int, int)));
	ui.twFileManager->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	ui.twFileManager->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	ui.twFileManager->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

	QHeaderView* header = ui.twFileManager->horizontalHeader();
	header->setSortIndicator(0, Qt::AscendingOrder);
	header->setSortIndicatorShown(true);
	connect(header, SIGNAL(sectionClicked(int)), ui.twFileManager, SLOT(sortByColumn(int)));

	m_network = new STNetworkClient;
	m_network->connectServer("10.1.0.10", "10001");
	connect(m_network, SIGNAL(processCloudFileMessage(QString)), this, SLOT(processMessage(QString)));
}

STCloudFileManager::~STCloudFileManager()
{
	m_network->disconnectServer();
}

void STCloudFileManager::processMessage(QString msg)
{
	QString type = msg.split(":")[0];
	msg.remove(0, type.size() + 1);
	if (type == "file_list")
	{
		makeCurrentPageTable(msg);
	}
	else if (type == "folder_list")
	{
		callCloudFolderView(msg);
	}
}

void STCloudFileManager::changeRowColor(int row, int column)
{
	QTableWidgetItem* item = 0;

	item = ui.twFileManager->item(previousColorRow, 0);
	if (item != 0)
	{
		this->setRowColor(previousColorRow, defaultBkColor);
	}

	item = ui.twFileManager->item(row, column);
	if (item != 0 && !item->isSelected() && !item->text().isEmpty())
	{
		this->setRowColor(row, QColor(228, 230, 238));
	}

	previousColorRow = row;
}

void STCloudFileManager::setRowColor(int row, QColor color)
{
	for (int col = 0; col < ui.twFileManager->columnCount(); col++)
	{
		QTableWidgetItem* item = ui.twFileManager->item(row, col);
		item->setBackgroundColor(color);
	}
}

void STCloudFileManager::resizeEvent(QResizeEvent *)
{
	resizeHeaders();
}

void STCloudFileManager::resizeHeaders()
{
	ui.twFileManager->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui.twFileManager->horizontalHeader()->setStretchLastSection(true);
	ui.twFileManager->horizontalHeader()->resizeSection(0, ui.twFileManager->width() - 320 - 1);
	ui.twFileManager->horizontalHeader()->resizeSection(1, 120);
	ui.twFileManager->horizontalHeader()->resizeSection(2, 200);
}

void STCloudFileManager::initCloudFileView()
{
	resizeHeaders();
	for (int i = 0; i < m_folderBtnList.size(); i++)
	{
		ui.widPathList->layout()->removeWidget(m_folderBtnList.at(i));
	}
	m_folderList.clear();
	m_folderBtnList.clear();
	m_folderBtnInfo.clear();

	QPushButton* btn;
	btn = new QPushButton();
	connect(btn, SIGNAL(clicked()), this, SLOT(onFolderClicked()));
	btn->setText(QStringLiteral("\\全部文件"));
	btn->setStyleSheet("QPushButton{color:rgb(67, 69, 85);"
		"font:10pt \"微软雅黑\";background-repeat:no-repeat;border:none;text-align:left;}"
		"QPushButton:hover:!pressed{color:rgb(140, 140, 140);}");
	m_folderList.append("");
	m_folderBtnList.append(btn);
	FileInfo folderInfo;
	folderInfo.id = 0;
	m_folderBtnInfo[btn] = folderInfo;
	ui.widPathList->layout()->addWidget(btn);

	refreshCurrentPageTable();
}

void STCloudFileManager::refreshCurrentPageTable()
{
	QString path;
	for (int i = 0; i < m_folderList.size(); i++)
	{
		path = path + m_folderList.at(i);
	}

	QString msg = QString("{\"type\":\"file\",\"action\":\"list\",\"data\":{\"path\":\"%1\"}}").arg(path);
	m_network->sendMessage(msg);
}

void STCloudFileManager::makeCurrentPageTable(QString data)
{
	ui.twFileManager->setRowCount(0);
	ui.twFileManager->clearContents();
	m_tableBtnInfo.clear();
	//QString data = "[{\"type\":\"folder\",\"name\":\"aaa\",\"size\":\"-\",\"time\":\"2017-12-16 12:15\"},{\"type\":\"file\",\"name\":\"bbb.ppt\",\"size\":\"24M\",\"time\":\"2017-01-21 20:29\"},{\"type\":\"file\",\"name\":\"fffffffff.txt\",\"size\":\"6K\",\"time\":\"2017-01-21 20:29\"},{\"type\":\"file\",\"name\":\"gggggggggggg.txt\",\"size\":\"257M\",\"time\":\"2018-02-01 10:53\"}]";

	QJsonArray files;
	QJsonParseError complexJsonError;
	QJsonDocument doucment = QJsonDocument::fromJson(data.toLatin1(), &complexJsonError);
	if (complexJsonError.error == QJsonParseError::NoError && doucment.isArray())
	{
		files = doucment.array();
	}

	QWidget* widget;
	QHBoxLayout* hLayout;
	QCheckBox* ckb;
	QLabel* lbl;
	QPushButton* btn;
	QString iconPath = ":/SoftTerminal/images/";
	QTableWidgetItem* sizeItem;
	QTableWidgetItem* timeItem;
	QJsonValue item;
	QJsonObject object;
	QString type;
	QString name;
	QString size;
	QString time;
	for (int i = 0; i < files.size(); i++)
	{
		item = files.at(i);
		if (item.isObject())
		{
			object = item.toObject();
			if (object.contains("type") && object["type"].isString())
			{
				type = object["type"].toString();
			}
			if (object.contains("name") && object["name"].isString())
			{
				name = object["name"].toString();
			}
			if (object.contains("size") && object["size"].isString())
			{
				size = object["size"].toString();
			}
			if (object.contains("time") && object["time"].isString())
			{
				time = object["time"].toString();
			}
		}

		ui.twFileManager->insertRow(i);

		iconPath = ":/SoftTerminal/images/";
		ckb = new QCheckBox();
		ckb->setStyleSheet("QCheckBox{spacing:5px;color:rgba(0, 0, 0, 0);}"
			"QCheckBox::indicator{width:20px;height:20px;}"
			"QCheckBox::indicator:unchecked{image:url(:/SoftTerminal/images/checkbox_unchecked.png);}"
			"QCheckBox::indicator:checked{image:url(:/SoftTerminal/images/checkbox_checked.png);}");
		ckb->setMaximumSize(QSize(20, 20));
		btn = new QPushButton();
		btn->setText(name);
		btn->setStyleSheet("QPushButton{color:rgb(67, 69, 85);"
			"font:10pt \"微软雅黑\";background-repeat:no-repeat;border:none;text-align:left;}"
			"QPushButton:hover:!pressed{color:rgb(140, 140, 140);}");
		if (type == "folder")
		{
			iconPath += "folder.png";
		}
		else
		{
			QString suffix = QFileInfo(name).suffix();
			if (suffix == "ppt" || suffix == "pptx" || suffix == "pptm")
			{
				iconPath += "ppt.png";
			}
			else if (suffix == "doc" || suffix == "docx")
			{
				iconPath += "doc.png";
			}
			else if (suffix == "pdf")
			{
				iconPath += "pdf.png";
			}
			else if (suffix == "jpg" || suffix == "jpeg" || suffix == "png" || suffix == "bmp")
			{
				iconPath += "pic.png";
			}
			else if (suffix == "txt")
			{
				iconPath += "txt.png";
			}
			else
			{
				iconPath += "file_unknown.png";
			}
		}
		btn->setIcon(QIcon(iconPath));
		btn->setIconSize(QSize(25, 25));
		connect(btn, SIGNAL(clicked()), this, SLOT(onFileItemClicked()));
		FileInfo info;
		info.type = type;
		info.name = name;
		m_tableBtnInfo[btn] = info;
		hLayout = new QHBoxLayout();
		widget = new QWidget(ui.twFileManager);
		widget->setObjectName("widget");
		widget->setStyleSheet("QWidget#widget{border-bottom: 1px solid #e3e3e3;}");
		hLayout->addWidget(ckb);
		hLayout->addWidget(btn);
		widget->setLayout(hLayout);
		ui.twFileManager->setCellWidget(i, 0, widget);

		lbl = new QLabel();
		lbl->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
		lbl->setStyleSheet("QLabel{color:rgb(67, 69, 85);font:10pt \"微软雅黑\";}");
		lbl->setText(size);
		hLayout = new QHBoxLayout();
		widget = new QWidget(ui.twFileManager);
		widget->setObjectName("widget");
		widget->setStyleSheet("QWidget#widget{border-bottom: 1px solid #e3e3e3;}");
		hLayout->addWidget(lbl);
		widget->setLayout(hLayout);
		ui.twFileManager->setCellWidget(i, 1, widget);


		lbl = new QLabel();
		lbl->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
		lbl->setStyleSheet("QLabel{color:rgb(67, 69, 85);font:10pt \"微软雅黑\";}");
		lbl->setText(time);
		hLayout = new QHBoxLayout();
		widget = new QWidget(ui.twFileManager);
		widget->setObjectName("widget");
		widget->setStyleSheet("QWidget#widget{border-bottom: 1px solid #e3e3e3;}");
		hLayout->addWidget(lbl);
		widget->setLayout(hLayout);
		ui.twFileManager->setCellWidget(i, 2, widget);
	}
}

void STCloudFileManager::onFileItemClicked()
{
	QPushButton* button = dynamic_cast<QPushButton*>(QObject::sender());
	if (m_tableBtnInfo.contains(button))
	{
		FileInfo info = m_tableBtnInfo[button];
		if (info.type == "file")
		{
			return;
		}
		else
		{
			QPushButton* btn;
			btn = new QPushButton();
			connect(btn, SIGNAL(clicked()), this, SLOT(onFolderClicked()));
			btn->setText(QString("\\") + info.name);
			btn->setStyleSheet("QPushButton{color:rgb(67, 69, 85);"
				"font:10pt \"微软雅黑\";background-repeat:no-repeat;border:none;text-align:left;}"
				"QPushButton:hover:!pressed{color:rgb(140, 140, 140);}");
			m_folderList.append(QString("/") + info.name);
			m_folderBtnList.append(btn);
			FileInfo folderInfo;
			folderInfo.id = info.id;
			m_folderBtnInfo[btn] = folderInfo;
			ui.widPathList->layout()->addWidget(btn);

			refreshCurrentPageTable();
		}
	}
}

void STCloudFileManager::onFolderClicked()
{
	QPushButton* button = dynamic_cast<QPushButton*>(QObject::sender());
	bool del = false;
	int index = 0;
	for (int i = 0; i < m_folderBtnList.size(); i++)
	{
		if (del)
		{
			m_folderBtnInfo.remove(m_folderBtnList.at(i));
			ui.widPathList->layout()->removeWidget(m_folderBtnList.at(i));
		}
		if (m_folderBtnList.at(i) == button)
		{
			del = true;
			index = i;
		}
	}
	while (m_folderBtnList.size() - 1 > index)
	{
		int size = m_folderBtnList.size();
		m_folderList.removeLast();
		m_folderBtnList.removeLast();
	}

	refreshCurrentPageTable();
}

void STCloudFileManager::on_pbUpload_clicked()
{
	STCloudUploadFile* uploadFile = new STCloudUploadFile(this);
	int parentX = mapToGlobal(pos()).x();
	int parentY = mapToGlobal(pos()).y();
	int parentWidth = width();
	int parentHeight = height();
	uploadFile->move(QPoint(parentX + (parentWidth - uploadFile->width()) / 2,
		parentY + (parentHeight - uploadFile->height()) / 2));
	uploadFile->exec();
}

void STCloudFileManager::on_pbNew_clicked()
{
	STCloudNewFolder* newFolder = new STCloudNewFolder(this);
	connect(newFolder, SIGNAL(confirmOK(QString)), this, SLOT(handleNewFile(QString)));
	int parentX = mapToGlobal(pos()).x();
	int parentY = mapToGlobal(pos()).y();
	int parentWidth = width();
	int parentHeight = height();
	newFolder->move(QPoint(parentX + (parentWidth - newFolder->width()) / 2,
		parentY + (parentHeight - newFolder->height()) / 2));
	newFolder->exec();
}

void STCloudFileManager::handleNewFile(QString name)
{
	QString path;
	for (int i = 0; i < m_folderList.size(); i++)
	{
		path = path + m_folderList.at(i);
	}
	QString msg = QString("{\"type\":\"file\",\"action\":\"new\","
		"\"data\":{\"path\":\"%1\",\"name\":\"%2\"}}").arg(path, name);
	m_network->sendMessage(msg);
}

void STCloudFileManager::on_pbCopy_clicked()
{
	getCheckList();
	if (m_checkedList.size() == 0)
	{
		return;
	}
	m_action = "copy";
	QString msg = QString("{\"type\":\"folder\",\"action\":\"list\"}");
	m_network->sendMessage(msg);
}

void STCloudFileManager::on_pbMove_clicked()
{
	getCheckList();
	if (m_checkedList.size() == 0)
	{
		return;
	}
	m_action = "move";
	QString msg = QString("{\"type\":\"folder\",\"action\":\"list\"}");
	m_network->sendMessage(msg);
}

void STCloudFileManager::callCloudFolderView(QString data)
{
	STCloudFolderView* folderView = new STCloudFolderView(m_action, data, this);
	connect(folderView, SIGNAL(confirmOK(QString, QString)), this, SLOT(handleFolderView(QString, QString)));
	int parentX = mapToGlobal(pos()).x();
	int parentY = mapToGlobal(pos()).y();
	int parentWidth = width();
	int parentHeight = height();
	folderView->move(QPoint(parentX + (parentWidth - folderView->width()) / 2,
		parentY + (parentHeight - folderView->height()) / 2));
	folderView->exec();
}

void STCloudFileManager::getCheckList()
{
	m_checkedList.clear();

	QWidget* widget;
	QCheckBox* ckb;
	QPushButton* btn;
	QObjectList list;
	QLayoutItem* item;
	for (int i = 0; i < ui.twFileManager->rowCount(); i++)
	{
		widget = ui.twFileManager->cellWidget(i, 0);
		item = widget->layout()->itemAt(0);
		ckb = qobject_cast<QCheckBox*>(item->widget());
		item = widget->layout()->itemAt(1);
		btn = qobject_cast<QPushButton*>(item->widget());

		if (ckb->isChecked())
		{
			m_checkedList.append(btn->text());
		}
	}
}

void STCloudFileManager::handleFolderView(QString action, QString destPath)
{
	QString path;
	for (int i = 0; i < m_folderList.size(); i++)
	{
		path = path + m_folderList.at(i);
	}
	QString msg;
	for (int i = 0; i < m_checkedList.size(); i++)
	{
		msg = QString("{\"type\":\"file\",\"action\":\"%1\","
			"\"data\":{\"path\":\"%2\",\"name\":\"%3\",\"destPath\":\"%4\"}}").arg(action, path, m_checkedList.at(i), destPath);
		m_network->sendMessage(msg);
	}
}

void STCloudFileManager::on_pbDel_clicked()
{
	getCheckList();
	if (m_checkedList.size() == 0)
	{
		return;
	}
	QString path;
	for (int i = 0; i < m_folderList.size(); i++)
	{
		path = path + m_folderList.at(i);
	}
	QString msg;
	for (int i = 0; i < m_checkedList.size(); i++)
	{
		msg = QString("{\"type\":\"file\",\"action\":\"del\","
			"\"data\":{\"path\":\"%1\",\"name\":\"%2\"}}").arg(path, m_checkedList.at(i));
		m_network->sendMessage(msg);
	}
}

void STCloudFileManager::on_pbSupport_clicked()
{
	m_support->hide();
	int parentX = mapToGlobal(pos()).x();
	int parentY = mapToGlobal(pos()).y();
	int parentWidth = width();
	int parentHeight = height();
	m_support->move(QPoint(parentX + (parentWidth - m_support->width() + 20),
		parentY + 50));
	m_support->show();
}

////////////////////////////////////////////////////////////////////////////////////////////////
STCloudUploadClient::STCloudUploadClient(QObject * parent) : QObject(parent)
{
	m_tcpSocket = NULL;
}

STCloudUploadClient::~STCloudUploadClient()
{

}

void STCloudUploadClient::connectServer(QString ip, QString port)
{
	delete(m_tcpSocket);
	m_tcpSocket = NULL;
	m_tcpSocket = new QTcpSocket(this); // 申请堆空间有TCP发送和接受操作
	m_tcpIp = ip;
	m_tcpPort = port;
	m_tcpSocket->connectToHost(m_tcpIp, m_tcpPort.toInt()); // 连接主机
	connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this,
		SLOT(displayError(QAbstractSocket::SocketError))); // 错误连接
	connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(sendFileInfo()));  //当连接成功时，就开始传送文件  
	connect(m_tcpSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(uploadFileData(qint64)));
}

void STCloudUploadClient::disconnectServer()
{
	disconnect(m_tcpSocket);
	m_tcpSocket->disconnect();
}

void STCloudUploadClient::displayError(QAbstractSocket::SocketError e)
{
	printf("error:%d", e);
}

void STCloudUploadClient::uploadFile(QString destPath, QString file)
{
	m_destPath = destPath;
	m_file = file;

	m_loadSize = 0;
	m_byteToWrite = 0;
	m_totalSize = 0;
	m_startUploadFileData = false;
	m_outBlock.clear();

	m_localFile = new QFile(m_file);
	m_localFile->open(QFile::ReadOnly);

	m_byteToWrite = m_localFile->size();  //剩余数据的大小  
	m_totalSize = m_localFile->size();

	m_loadSize = 512 * 1024;  //每次发送数据的大小  

	connectServer("10.1.0.10", "10002");
}

void STCloudUploadClient::sendFileInfo()
{
	QString destFilePath = m_destPath + "/" + m_file.right(m_file.size() - m_file.lastIndexOf('/') - 1);
	m_tcpSocket->write("#*#" + destFilePath.toUtf8() + "@%@");
}

void STCloudUploadClient::uploadFileData(qint64 numBytes)  //开始发送文件内容
{
	if (m_byteToWrite == 0)
	{
		disconnectServer();
		return;
	}
	if (!m_startUploadFileData)
	{
		numBytes = 0;
		m_startUploadFileData = true;
	}
	m_byteToWrite -= numBytes;  //剩余数据大小
	m_outBlock = m_localFile->read(qMin(m_byteToWrite, m_loadSize));
	m_tcpSocket->write(m_outBlock);

	if (m_byteToWrite == 0)
	{
		m_tcpSocket->write("#*#finish@%@");
		//发送完毕
		Q_EMIT onUploadFinished();
	}
	else
	{
		int percent = (m_totalSize - m_byteToWrite) * 100 / m_totalSize;
		Q_EMIT onUploadProcess(percent);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
STCloudSupport::STCloudSupport(QWidget * parent) : QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Popup);
}

STCloudSupport::~STCloudSupport()
{

}

bool STCloudSupport::event(QEvent* event)
{
	// class_ameneded 不能是custommenu的成员, 因为winidchange事件触发时, 类成员尚未初始化  
	static bool class_amended = false;
	if (event->type() == QEvent::WinIdChange)
	{
		HWND hwnd = reinterpret_cast<HWND>(winId());
		if (class_amended == false)
		{
			class_amended = true;
			DWORD class_style = ::GetClassLong(hwnd, GCL_STYLE);
			class_style &= ~CS_DROPSHADOW;
			::SetClassLong(hwnd, GCL_STYLE, class_style); // windows系统函数  
		}
	}
	return QWidget::event(event);
}

////////////////////////////////////////////////////////////////////////////////////////////////
STCloudUploadFile::STCloudUploadFile(QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);
	//setWindowModality(Qt::ApplicationModal);
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

	ui.swUpload->setCurrentIndex(0);

	m_uploadClient = new STCloudUploadClient();
	connect(m_uploadClient, SIGNAL(onUploadFinished()), this, SLOT(onUploadFinished()));
	connect(m_uploadClient, SIGNAL(onUploadProcess(int)), this, SLOT(onUploadProcess(int)));
}

STCloudUploadFile::~STCloudUploadFile()
{

}

void STCloudUploadFile::on_pbOK_clicked()
{
	close();
}

void STCloudUploadFile::on_pbGoon_clicked()
{
	on_pbUpload_clicked();
}

void STCloudUploadFile::on_pbCancel_clicked()
{
	ui.swUpload->setCurrentIndex(0);
}

void STCloudUploadFile::on_pbClose_clicked()
{
	close();
}

void STCloudUploadFile::on_pbUpload_clicked()
{
	QString path = QFileDialog::getOpenFileName(this,
		QString("打开"), QString(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)),
		QString());

	if (path.isEmpty() || !QFile::exists(path))
	{
		return;
	}
	m_uploadClient->uploadFile("/", path);

	ui.pbUploadStatus->setValue(0);
	ui.swUpload->setCurrentIndex(1);
	ui.pbOK->setVisible(false);
	ui.pbGoon->setVisible(false);
	ui.pbCancel->setVisible(true);
}

void STCloudUploadFile::onUploadFinished()
{
	ui.pbUploadStatus->setValue(100);
	ui.pbOK->setVisible(true);
	ui.pbGoon->setVisible(true);
	ui.pbCancel->setVisible(false);
}

void STCloudUploadFile::onUploadProcess(int percent)
{
	ui.pbUploadStatus->setValue(percent);
}

void STCloudUploadFile::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}
}

void STCloudUploadFile::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint pos = this->pos() + movePoint;
		m_startMovePos = event->globalPos();
		this->move(pos.x(), pos.y());
	}
}

void STCloudUploadFile::mouseReleaseEvent(QMouseEvent* event)
{
	m_isPressed = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
STCloudNewFolder::STCloudNewFolder(QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);
	//setWindowModality(Qt::ApplicationModal);
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	ui.leFolderName->setFocus();
}

STCloudNewFolder::~STCloudNewFolder()
{

}

void STCloudNewFolder::on_pbOK_clicked()
{
	QString path;
	path = ui.leFolderName->text();
	Q_EMIT confirmOK(path);
	close();
}

void STCloudNewFolder::on_pbCancel_clicked()
{
	close();
}

void STCloudNewFolder::on_pbClose_clicked()
{
	close();
}

void STCloudNewFolder::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}
}

void STCloudNewFolder::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint pos = this->pos() + movePoint;
		m_startMovePos = event->globalPos();
		this->move(pos.x(), pos.y());
	}
}

void STCloudNewFolder::mouseReleaseEvent(QMouseEvent* event)
{
	m_isPressed = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
STCloudFolderView::STCloudFolderView(QString action, QString folders, QWidget * parent)
	: QDialog(parent), m_action(action), m_folders(folders)
{
	ui.setupUi(this);
	//setWindowModality(Qt::ApplicationModal);
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	if (m_action == "copy")
	{
		ui.lblTitle->setText(QStringLiteral("所选内容复制到"));
	}
	else
	{
		ui.lblTitle->setText(QStringLiteral("所选内容移动到"));
	}
	initFolderTree();
}

STCloudFolderView::~STCloudFolderView()
{

}

void STCloudFolderView::initFolderTree()
{
	m_folders = "[{\"name\": \"aaa\", \"child\": [{\"name\": \"fff\", \"child\": [{\"name\": \"sdfasfd\", \"child\": []}]}, {\"name\": \"ddd\", \"child\": []}]}, {\"name\": \"ccc\", \"child\": [{\"name\": \"bbb\", \"child\": []}]}]";

	QStandardItemModel* model;
	model = new QStandardItemModel(0, 1);
	model->setHorizontalHeaderLabels(QStringList() << QString::fromLocal8Bit("名称") << QString::fromLocal8Bit("路径"));
	QStandardItem* treeItem;
	treeItem = new QStandardItem(QIcon(":/SoftTerminal/images/folder.png"), QStringLiteral("全部文件"));

	QJsonArray files;
	QJsonParseError complexJsonError;
	QJsonDocument doucment = QJsonDocument::fromJson(m_folders.toLatin1(), &complexJsonError);
	if (complexJsonError.error == QJsonParseError::NoError && doucment.isArray())
	{
		files = doucment.array();
		makeFolderSubTree(treeItem, files, "");
	}
	model->appendRow(treeItem);
	model->setItem(0, 1, new QStandardItem(""));
	ui.tvFolder->setModel(model);
	ui.tvFolder->expandAll();
	ui.tvFolder->setHeaderHidden(true);
	ui.tvFolder->setColumnHidden(1, true);
	ui.tvFolder->setColumnWidth(0, ui.tvFolder->width());
}

void STCloudFolderView::makeFolderSubTree(QStandardItem* parent, QJsonArray arr, QString parentPath)
{
	QJsonValue item;
	QJsonObject object;
	QString name;
	QStandardItem* treeItem;
	QString path;
	for (int i = 0; i < arr.size(); i++)
	{
		item = arr.at(i);
		if (item.isObject())
		{
			object = item.toObject();
			if (object.contains("name") && object["name"].isString())
			{
				name = object["name"].toString();
				treeItem = new QStandardItem(QIcon(":/SoftTerminal/images/folder.png"), name);
				parent->appendRow(treeItem);
				path = parentPath + "/" + name;
				parent->setChild(i, 1, new QStandardItem(path));
				if (object.contains("child") && object["child"].isArray())
				{
					makeFolderSubTree(treeItem, object["child"].toArray(), path);
				}
			}
		}
	}
}

void STCloudFolderView::on_pbOK_clicked()
{
	QString path;
	QModelIndex temp = ui.tvFolder->currentIndex();
	QModelIndex pathIdx = temp.sibling(temp.row(), 1);
	path = ui.tvFolder->model()->itemData(pathIdx).values()[0].toString();

	Q_EMIT confirmOK(m_action, path);
	close();
}

void STCloudFolderView::on_pbCancel_clicked()
{
	close();
}

void STCloudFolderView::on_pbClose_clicked()
{
	close();
}

void STCloudFolderView::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}
}

void STCloudFolderView::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint pos = this->pos() + movePoint;
		m_startMovePos = event->globalPos();
		this->move(pos.x(), pos.y());
	}
}

void STCloudFolderView::mouseReleaseEvent(QMouseEvent* event)
{
	m_isPressed = false;
}
