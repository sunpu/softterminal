#include "stwbcloudfileview.h"

using namespace tahiti;

STWBCloudFileView::STWBCloudFileView(QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

	ui.twFileManager->setMouseTracking(true);
	ui.twFileManager->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	ui.twFileManager->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	ui.twFileManager->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

	m_messageClient = new STMessageClient;
	connect(m_messageClient, SIGNAL(cloudFileMessageSignal(QString)), this, SLOT(processMessage(QString)));
}

STWBCloudFileView::~STWBCloudFileView()
{
}

void STWBCloudFileView::processMessage(QString msg)
{
	QString type = msg.split(":")[0];
	msg.remove(0, type.size() + 1);
	if (type == "file_list")
	{
		makeCurrentPageTable(msg);
	}
}

void STWBCloudFileView::resizeEvent(QResizeEvent *)
{
	resizeHeaders();
}

void STWBCloudFileView::resizeHeaders()
{
	ui.twFileManager->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui.twFileManager->horizontalHeader()->setStretchLastSection(true);
	ui.twFileManager->horizontalHeader()->resizeSection(0, ui.twFileManager->width() - 320 - 1);
	ui.twFileManager->horizontalHeader()->resizeSection(1, 120);
	ui.twFileManager->horizontalHeader()->resizeSection(2, 200);
}

void STWBCloudFileView::initCloudFileView()
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
	btn->setStyleSheet("QPushButton{color:#c7c7c7;"
		"font:10pt \"微软雅黑\";background-repeat:no-repeat;border:none;text-align:left;}"
		"QPushButton:hover:!pressed{color:#ffffff;}");
	m_folderList.append("");
	m_folderBtnList.append(btn);
	FileInfo folderInfo;
	folderInfo.id = 0;
	m_folderBtnInfo[btn] = folderInfo;
	ui.widPathList->layout()->addWidget(btn);

	refreshCurrentPageTable();
}

void STWBCloudFileView::refreshCurrentPageTable()
{
	QString path;
	for (int i = 0; i < m_folderList.size(); i++)
	{
		path = path + m_folderList.at(i);
	}

	QString msg = QString("{\"type\":\"file\",\"action\":\"list\",\"data\":{\"path\":\"%1\"}}").arg(path);
	m_messageClient->sendMessage(msg);
}

void STWBCloudFileView::makeCurrentPageTable(QString data)
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
		btn = new QPushButton();
		btn->setText(name);
		btn->setStyleSheet("QPushButton{color:#c7c7c7;"
			"font:10pt \"微软雅黑\";background-repeat:no-repeat;border:none;text-align:left;}"
			"QPushButton:hover:!pressed{color:#ffffff;}");
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
		widget->setStyleSheet("QWidget#widget{border-bottom: 1px solid #494c54;}");
		hLayout->addWidget(btn);
		widget->setLayout(hLayout);
		ui.twFileManager->setCellWidget(i, 0, widget);

		lbl = new QLabel();
		lbl->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
		lbl->setStyleSheet("QLabel{color:#c7c7c7;font:10pt \"微软雅黑\";}");
		lbl->setText(size);
		hLayout = new QHBoxLayout();
		widget = new QWidget(ui.twFileManager);
		widget->setObjectName("widget");
		widget->setStyleSheet("QWidget#widget{border-bottom: 1px solid #494c54;}");
		hLayout->addWidget(lbl);
		widget->setLayout(hLayout);
		ui.twFileManager->setCellWidget(i, 1, widget);


		lbl = new QLabel();
		lbl->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
		lbl->setStyleSheet("QLabel{color:#c7c7c7;font:10pt \"微软雅黑\";}");
		lbl->setText(time);
		hLayout = new QHBoxLayout();
		widget = new QWidget(ui.twFileManager);
		widget->setObjectName("widget");
		widget->setStyleSheet("QWidget#widget{border-bottom: 1px solid #494c54;}");
		hLayout->addWidget(lbl);
		widget->setLayout(hLayout);
		ui.twFileManager->setCellWidget(i, 2, widget);
	}
}

void STWBCloudFileView::onFileItemClicked()
{
	QPushButton* button = dynamic_cast<QPushButton*>(QObject::sender());
	if (m_tableBtnInfo.contains(button))
	{
		FileInfo info = m_tableBtnInfo[button];
		if (info.type == "file")
		{
			QString path;
			for (int i = 0; i < m_folderList.size(); i++)
			{
				path = path + m_folderList.at(i);
			}
			path = path + "/" + info.name;
			Q_EMIT openCloudFileSignal(path);
			on_pbClose_clicked();
		}
		else
		{
			QPushButton* btn;
			btn = new QPushButton();
			connect(btn, SIGNAL(clicked()), this, SLOT(onFolderClicked()));
			btn->setText(QString("\\") + info.name);
			btn->setStyleSheet("QPushButton{color:#c7c7c7;"
				"font:10pt \"微软雅黑\";background-repeat:no-repeat;border:none;text-align:left;}"
				"QPushButton:hover:!pressed{color:#ffffff;}");
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

void STWBCloudFileView::onFolderClicked()
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

void STWBCloudFileView::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}
}

void STWBCloudFileView::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint pos = this->pos() + movePoint;
		m_startMovePos = event->globalPos();
		this->move(pos.x(), pos.y());
	}
}

void STWBCloudFileView::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Escape:
		break;
	default:
		QDialog::keyPressEvent(event);
	}
}

void STWBCloudFileView::mouseReleaseEvent(QMouseEvent* event)
{
	m_isPressed = false;
}

void STWBCloudFileView::on_pbReload_clicked()
{
	refreshCurrentPageTable();
}

void STWBCloudFileView::on_pbClose_clicked()
{
	Q_EMIT closeCloudFileView();
}