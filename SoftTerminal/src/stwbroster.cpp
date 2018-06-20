#include "stwbroster.h"

using namespace tahiti;

STWBRoster::STWBRoster(QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

	ui.twRoster->setMouseTracking(true);
	ui.twRoster->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	ui.twRoster->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	ui.twRoster->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	ui.twRoster->horizontalHeaderItem(3)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	ui.twRoster->horizontalHeaderItem(4)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	ui.twRoster->horizontalHeaderItem(5)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

	QHBoxLayout* mainLayout = new QHBoxLayout;
	QPushButton* searchBtn = new QPushButton;
	searchBtn->setFixedSize(13, 13);
	searchBtn->setStyleSheet("QPushButton{border-image:url(:/SoftTerminal/images/search.png);"
		"background:transparent;cursor:pointer;}");
	//setPlaceholderText(tr("搜索"));
	mainLayout->addWidget(searchBtn);

	mainLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	m_clearBtn = new QPushButton;
	m_clearBtn->setFixedSize(13, 13);
	m_clearBtn->setCursor(Qt::PointingHandCursor);
	m_clearBtn->setStyleSheet("QPushButton{border-image:url(:/SoftTerminal/images/clear.png);"
		"background:transparent;}");
	mainLayout->addWidget(m_clearBtn);

	mainLayout->setContentsMargins(8, 0, 8, 0);
	ui.leSearch->setTextMargins(25, 0, 25, 0);
	ui.leSearch->setContentsMargins(0, 0, 0, 0);
	ui.leSearch->setLayout(mainLayout);
	connect(ui.leSearch, SIGNAL(textChanged(const QString)), this, SLOT(onTextChanged()));
	connect(m_clearBtn, SIGNAL(clicked()), this, SLOT(clearSearchInput()));
	m_clearBtn->setVisible(false);
	ui.leSearch->installEventFilter(this);

	m_messageClient = new STMessageClient;
}

STWBRoster::~STWBRoster() {
	
}

void STWBRoster::onTextChanged()
{
}

void STWBRoster::clearSearchInput()
{
	ui.leSearch->clear();
	m_clearBtn->setVisible(false);
}

void STWBRoster::resizeEvent(QResizeEvent *)
{
	resizeHeaders();
}

void STWBRoster::resizeHeaders()
{
	ui.twRoster->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui.twRoster->horizontalHeader()->setStretchLastSection(true);
	ui.twRoster->horizontalHeader()->resizeSection(0, ui.twRoster->width() - 450 - 1);
	ui.twRoster->horizontalHeader()->resizeSection(1, 90);
	ui.twRoster->horizontalHeader()->resizeSection(2, 90);
	ui.twRoster->horizontalHeader()->resizeSection(3, 90);
	ui.twRoster->horizontalHeader()->resizeSection(4, 90);
	ui.twRoster->horizontalHeader()->resizeSection(5, 90);
}

void STWBRoster::initRoster(QString courseID, QString admin, bool isAdmin)
{
	m_courseID = courseID;
	m_isAdmin = isAdmin;

	ui.lblTeacherName->setText(admin);

	resizeHeaders();

	ui.twRoster->setRowCount(0);
	ui.twRoster->clearContents();

	refreshRosterTable();
}

void STWBRoster::refreshRosterTable()
{
	// {"type":"course","action":"queryRoster","courseID":"111"}
	QString msg = QString("{\"type\":\"course\",\"action\":\"queryRoster\",\"courseID\":\"%1\"}").arg(m_courseID);
	QString data = m_messageClient->sendMessage(msg);

	ui.twRoster->setRowCount(0);
	ui.twRoster->clearContents();
	m_btnInfoMap.clear();
	//data = "[{\"jid\":\"a\",\"name\":\"aaa\",\"show\":\"true\",\"operate\":\"false\",\"mic\":\"true\",\"camera\":\"false\"},{\"jid\":\"b\",\"name\":\"bbb\",\"show\":\"false\",\"operate\":\"false\",\"mic\":\"false\",\"camera\":\"false\"},{\"jid\":\"c\",\"name\":\"ccc\",\"show\":\"true\",\"operate\":\"true\",\"mic\":\"false\",\"camera\":\"false\"},{\"jid\":\"d\",\"name\":\"ddd\",\"show\":\"true\",\"operate\":\"false\",\"mic\":\"true\",\"camera\":\"true\"}]";

	QWidget* widget;
	QHBoxLayout* hLayout;
	QLabel* lbl;
	QPushButton* showBtn;
	QPushButton* operateBtn;
	QPushButton* cameraBtn;
	QPushButton* micBtn;
	QPushButton* handBtn;
	QJsonArray members;
	QJsonValue item;
	QJsonObject object;
	QString jid;
	QString name;
	bool show;
	bool operate;
	QString mic;
	QString camera;
	BtnInfo info;
	QJsonParseError complexJsonError;
	QJsonDocument doucment = QJsonDocument::fromJson(data.toLatin1(), &complexJsonError);
	if (complexJsonError.error == QJsonParseError::NoError && doucment.isArray())
	{
		members = doucment.array();
		for (int i = 0; i < members.size(); i++)
		{
			item = members.at(i);
			if (item.isObject())
			{
				object = item.toObject();
				if (object.contains("jid") && object["jid"].isString())
				{
					jid = object["jid"].toString();
				}
				if (object.contains("name") && object["name"].isString())
				{
					name = object["name"].toString();
				}
				if (object.contains("show") && object["show"].isString())
				{
					show = (object["show"].toString().compare("true") == 0);
				}
				if (object.contains("operate") && object["operate"].isString())
				{
					operate = (object["operate"].toString().compare("true") == 0);
				}
				if (object.contains("camera") && object["camera"].isString())
				{
					camera = object["camera"].toString();
				}
				if (object.contains("mic") && object["mic"].isString())
				{
					mic = object["mic"].toString();
				}
			}

			ui.twRoster->insertRow(i);

			lbl = new QLabel();
			lbl->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
			lbl->setStyleSheet("QLabel{color:#ffffff;font:10pt \"微软雅黑\";}");
			lbl->setText(name);
			hLayout = new QHBoxLayout();
			widget = new QWidget(ui.twRoster);
			widget->setObjectName("widget");
			widget->setStyleSheet("QWidget#widget{border-bottom: 1px solid #494c54;}");
			hLayout->addWidget(lbl);
			widget->setLayout(hLayout);
			ui.twRoster->setCellWidget(i, 0, widget);

			operateBtn = new QPushButton();
			operateBtn->setFixedSize(20, 20);
			if (!show)
			{
				operateBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/operate_disable.png);");
			}
			else
			{
				if (operate)
				{
					operateBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/operate_on.png);");
				}
				else
				{
					operateBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/operate_off.png);");
				}
			}
			if (m_isAdmin)
			{
				connect(operateBtn, SIGNAL(clicked()), this, SLOT(onBtnClicked()));
			}
			info.type = "operate";
			info.jid = jid;
			info.flag = operate;
			info.enable = true;
			m_btnInfoMap[operateBtn] = info;
			hLayout = new QHBoxLayout();
			widget = new QWidget(ui.twRoster);
			widget->setObjectName("widget");
			widget->setStyleSheet("QWidget#widget{border-bottom: 1px solid #494c54;}");
			hLayout->addWidget(operateBtn);
			widget->setLayout(hLayout);
			ui.twRoster->setCellWidget(i, 2, widget);

			cameraBtn = new QPushButton();
			cameraBtn->setFixedSize(18, 20);
			if (!show)
			{
				cameraBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/camera_disable.png);");
			}
			else
			{
				if (camera == "true")
				{
					cameraBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/camera_on.png);");
				}
				else if (camera == "false")
				{
					cameraBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/camera_off.png);");
				}
				else
				{
					cameraBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/camera_disable.png);");
				}
			}
			info.type = "camera";
			info.jid = jid;
			if (camera == "disable")
			{
				info.enable = false;
			}
			else
			{
				info.flag = (camera == "true");
			}
			m_btnInfoMap[cameraBtn] = info;
			hLayout = new QHBoxLayout();
			widget = new QWidget(ui.twRoster);
			widget->setObjectName("widget");
			widget->setStyleSheet("QWidget#widget{border-bottom: 1px solid #494c54;}");
			hLayout->addWidget(cameraBtn);
			widget->setLayout(hLayout);
			ui.twRoster->setCellWidget(i, 3, widget);

			micBtn = new QPushButton();
			micBtn->setFixedSize(30, 20);
			if (!show)
			{
				micBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/mic_disable.png);");
			}
			else
			{
				if (mic == "true")
				{
					micBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/mic_on.png);");
				}
				else if (mic == "false")
				{
					micBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/mic_off.png);");
				}
				else
				{
					micBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/mic_disable.png);");
				}
			}
			if (m_isAdmin)
			{
				connect(micBtn, SIGNAL(clicked()), this, SLOT(onBtnClicked()));
			}
			info.type = "mic";
			info.jid = jid;
			if (mic == "disable")
			{
				info.enable = false;
			}
			else
			{
				info.flag = (mic == "true");
			}
			m_btnInfoMap[micBtn] = info;
			hLayout = new QHBoxLayout();
			widget = new QWidget(ui.twRoster);
			widget->setObjectName("widget");
			widget->setStyleSheet("QWidget#widget{border-bottom: 1px solid #494c54;}");
			hLayout->addWidget(micBtn);
			widget->setLayout(hLayout);
			ui.twRoster->setCellWidget(i, 4, widget);

			handBtn = new QPushButton();
			handBtn->setFixedSize(20, 20);
			handBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/hand.png);");
			hLayout = new QHBoxLayout();
			widget = new QWidget(ui.twRoster);
			widget->setObjectName("widget");
			widget->setStyleSheet("QWidget#widget{border-bottom: 1px solid #494c54;}");
			hLayout->addWidget(handBtn);
			widget->setLayout(hLayout);
			ui.twRoster->setCellWidget(i, 5, widget);

			showBtn = new QPushButton();
			showBtn->setFixedSize(20, 20);
			if (show)
			{
				showBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/show_on.png);");
			}
			else
			{
				showBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/show_off.png);");
			}
			if (m_isAdmin)
			{
				connect(showBtn, SIGNAL(clicked()), this, SLOT(onBtnClicked()));
			}
			info.type = "show";
			info.jid = jid;
			info.flag = show;
			info.enable = true;
			info.operateBtn = operateBtn;
			info.cameraBtn = cameraBtn;
			info.micBtn = micBtn;
			m_btnInfoMap[showBtn] = info;
			hLayout = new QHBoxLayout();
			widget = new QWidget(ui.twRoster);
			widget->setObjectName("widget");
			widget->setStyleSheet("QWidget#widget{border-bottom: 1px solid #494c54;}");
			hLayout->addWidget(showBtn);
			widget->setLayout(hLayout);
			ui.twRoster->setCellWidget(i, 1, widget);

			ui.twRoster->setRowHeight(i, 35);
		}
	}
}

void STWBRoster::onBtnClicked()
{
	QPushButton* button = dynamic_cast<QPushButton*>(QObject::sender());
	if (m_btnInfoMap.contains(button))
	{
		BtnInfo info = m_btnInfoMap[button];
		if (!info.enable)
		{
			return;
		}
		info.flag = !info.flag;
		m_btnInfoMap[button] = info;
		updateBtnImage(button, info);
		Q_EMIT setAuthorityStatusSignal(info.type, info.jid, info.flag);
	}
}

void STWBRoster::updateBtnImage(QPushButton* button, BtnInfo info)
{
	if (info.type == "show")
	{
		if (info.flag)
		{
			button->setStyleSheet("border-image: url(:/SoftTerminal/images/show_on.png);");
			if (m_btnInfoMap[info.operateBtn].flag)
			{
				info.operateBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/operate_on.png);");
			}
			else
			{
				info.operateBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/operate_off.png);");
			}
			if (m_btnInfoMap[info.cameraBtn].enable)
			{
				if (m_btnInfoMap[info.cameraBtn].flag)
				{
					info.cameraBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/camera_on.png);");
				}
				else
				{
					info.cameraBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/camera_off.png);");
				}
			}
			if (m_btnInfoMap[info.micBtn].enable)
			{
				if (m_btnInfoMap[info.micBtn].flag)
				{
					info.micBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/mic_on.png);");
				}
				else
				{
					info.micBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/mic_off.png);");
				}
			}
		}
		else
		{
			button->setStyleSheet("border-image: url(:/SoftTerminal/images/show_off.png);");
			info.operateBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/operate_disable.png);");
			info.cameraBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/camera_disable.png);");
			info.micBtn->setStyleSheet("border-image: url(:/SoftTerminal/images/mic_disable.png);");
		}
	}
	else if (info.type == "operate")
	{
		if (info.flag)
		{
			button->setStyleSheet("border-image: url(:/SoftTerminal/images/operate_on.png);");
		}
		else
		{
			button->setStyleSheet("border-image: url(:/SoftTerminal/images/operate_off.png);");
		}
	}
	else if (info.type == "mic")
	{
		if (info.flag)
		{
			button->setStyleSheet("border-image: url(:/SoftTerminal/images/mic_on.png);");
		}
		else
		{
			button->setStyleSheet("border-image: url(:/SoftTerminal/images/mic_off.png);");
		}
	}
}

void STWBRoster::updateAuthorityStatusSlot(QString subtype, QString jid, bool flag)
{
	QMap<QPushButton*, BtnInfo>::Iterator it;
	for (it = m_btnInfoMap.begin(); it != m_btnInfoMap.end(); it++)
	{
		if (it.value().jid == jid && it.value().type == subtype)
		{
			QPushButton* button = it.key();
			BtnInfo info = m_btnInfoMap[button];
			if (!info.enable || info.flag == flag)
			{
				return;
			}

			info.flag = flag;
			m_btnInfoMap[button] = info;
			updateBtnImage(button, info);
		}
	}
}

void STWBRoster::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}
}

void STWBRoster::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint pos = this->pos() + movePoint;
		m_startMovePos = event->globalPos();
		this->move(pos.x(), pos.y());
	}
}

void STWBRoster::mouseReleaseEvent(QMouseEvent* event)
{
	m_isPressed = false;
}

void STWBRoster::on_pbClose_clicked()
{
	Q_EMIT closeRoster();
}

void STWBRoster::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Escape:
		break;
	default:
		QDialog::keyPressEvent(event);
	}
}

bool STWBRoster::eventFilter(QObject* obj, QEvent* e)
{
	if (ui.leSearch == obj)
	{
		if (e->type() == QEvent::FocusIn)
		{
			m_clearBtn->setVisible(true);
		}
		else if (e->type() == QEvent::FocusOut)
		{
			clearSearchInput();
		}
	}
	return false;
}
