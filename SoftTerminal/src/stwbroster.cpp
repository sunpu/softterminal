#include "stwbroster.h"

STWBRoster::STWBRoster(XmppClient* client, QWidget * parent)
	: m_xmppClient(client), QDialog(parent)
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

void STWBRoster::initRoster()
{
	resizeHeaders();

	ui.twRoster->setRowCount(0);
	ui.twRoster->clearContents();

	QWidget* widget;
	QHBoxLayout* hLayout;
	QLabel* lbl;
	QPushButton* btn;
	QString iconPath = ":/SoftTerminal/images/";
	QTableWidgetItem* sizeItem;
	QTableWidgetItem* timeItem;
	for (int i = 0; i < 3; i++)
	{
		ui.twRoster->insertRow(i);

		lbl = new QLabel();
		lbl->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
		lbl->setStyleSheet("QLabel{color:#ffffff;font:10pt \"微软雅黑\";}");
		lbl->setText("hahaha");
		hLayout = new QHBoxLayout();
		widget = new QWidget(ui.twRoster);
		widget->setObjectName("widget");
		widget->setStyleSheet("QWidget#widget{border-bottom: 1px solid #494c54;}");
		hLayout->addWidget(lbl);
		widget->setLayout(hLayout);
		ui.twRoster->setCellWidget(i, 0, widget);

		btn = new QPushButton();
		btn->setFixedSize(20, 20);
		btn->setStyleSheet("border-image: url(:/SoftTerminal/images/talk_on.png);");
		connect(btn, SIGNAL(clicked()), this, SLOT(onFileItemClicked()));
		hLayout = new QHBoxLayout();
		widget = new QWidget(ui.twRoster);
		widget->setObjectName("widget");
		widget->setStyleSheet("QWidget#widget{border-bottom: 1px solid #494c54;}");
		hLayout->addWidget(btn);
		widget->setLayout(hLayout);
		ui.twRoster->setCellWidget(i, 1, widget);

		btn = new QPushButton();
		btn->setFixedSize(20, 20);
		btn->setStyleSheet("border-image: url(:/SoftTerminal/images/control_disable.png);");
		connect(btn, SIGNAL(clicked()), this, SLOT(onFileItemClicked()));
		hLayout = new QHBoxLayout();
		widget = new QWidget(ui.twRoster);
		widget->setObjectName("widget");
		widget->setStyleSheet("QWidget#widget{border-bottom: 1px solid #494c54;}");
		hLayout->addWidget(btn);
		widget->setLayout(hLayout);
		ui.twRoster->setCellWidget(i, 2, widget);

		btn = new QPushButton();
		btn->setFixedSize(20, 20);
		btn->setStyleSheet("border-image: url(:/SoftTerminal/images/cam_off.png);");
		connect(btn, SIGNAL(clicked()), this, SLOT(onFileItemClicked()));
		hLayout = new QHBoxLayout();
		widget = new QWidget(ui.twRoster);
		widget->setObjectName("widget");
		widget->setStyleSheet("QWidget#widget{border-bottom: 1px solid #494c54;}");
		hLayout->addWidget(btn);
		widget->setLayout(hLayout);
		ui.twRoster->setCellWidget(i, 3, widget);

		btn = new QPushButton();
		btn->setFixedSize(20, 20);
		btn->setStyleSheet("border-image: url(:/SoftTerminal/images/recorder_disable.png);");
		connect(btn, SIGNAL(clicked()), this, SLOT(onFileItemClicked()));
		hLayout = new QHBoxLayout();
		widget = new QWidget(ui.twRoster);
		widget->setObjectName("widget");
		widget->setStyleSheet("QWidget#widget{border-bottom: 1px solid #494c54;}");
		hLayout->addWidget(btn);
		widget->setLayout(hLayout);
		ui.twRoster->setCellWidget(i, 4, widget);

		btn = new QPushButton();
		btn->setFixedSize(20, 20);
		btn->setStyleSheet("border-image: url(:/SoftTerminal/images/hand_on.png);");
		connect(btn, SIGNAL(clicked()), this, SLOT(onFileItemClicked()));
		hLayout = new QHBoxLayout();
		widget = new QWidget(ui.twRoster);
		widget->setObjectName("widget");
		widget->setStyleSheet("QWidget#widget{border-bottom: 1px solid #494c54;}");
		hLayout->addWidget(btn);
		widget->setLayout(hLayout);
		ui.twRoster->setCellWidget(i, 5, widget);

		ui.twRoster->setRowHeight(i, 35);
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
