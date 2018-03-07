#include "stwhiteboard.h"

using namespace tahiti;

STWhiteBoard::STWhiteBoard(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	ui.pbNormal->setVisible(false);

	// ��Ƶ���
	initConferenceClient();
	login();

	QObject::connect(this, SIGNAL(newStreamSignal(QString, int, int)), this, SLOT(newStreamSlot(QString, int, int)));

	qRegisterMetaType<std::shared_ptr<RemoteStream>>("std::shared_ptr<RemoteStream>");
	QObject::connect(this, SIGNAL(attachRenderSignal(QString, std::shared_ptr<RemoteStream>)), this, SLOT(attachRenderSlot(QString, std::shared_ptr<RemoteStream>)));
	QObject::connect(this, SIGNAL(detachRenderSignal(QString)), this, SLOT(detachRenderSlot(QString)));

	QHBoxLayout* hboxLayout = (QHBoxLayout*)ui.widVideo->layout();
	m_videoItems.resize(MAX_STREAM_NUM);
	for (size_t i = 0; i < MAX_STREAM_NUM; i++)
	{
		m_videoItems[i] = new STWBVideoItem(ui.widVideo);
		m_videoItems[i]->setObjectName(QString::number(i));
		m_videoItems[i]->setVisible(false);
		hboxLayout->addWidget(m_videoItems[i]);
	}

	// �װ����
	qRegisterMetaType<QVector<QPoint>>("QVector<QPoint>");
	qRegisterMetaType<QPoint>("QPoint");
	qRegisterMetaType<QList<int>>("QList<int>");

	m_network = new STNetworkClient();
	connect(m_network, SIGNAL(onConnect()), this, SLOT(connectNetworkServer()));
	connect(m_network, SIGNAL(drawRemoteRealtimePen(QString, int, QVector<QPoint>)),
		this, SLOT(drawRemoteRealtimePen(QString, int, QVector<QPoint>)));
	connect(m_network, SIGNAL(drawRemotePenItem(QString, int, QVector<QPoint>, int)),
		this, SLOT(drawRemotePenItem(QString, int, QVector<QPoint>, int)));
	connect(m_network, SIGNAL(drawRemoteTextItem(QString, int, QString, QPoint, int)),
		this, SLOT(drawRemoteTextItem(QString, int, QString, QPoint, int)));
	connect(m_network, SIGNAL(moveRemoteItems(QPoint, int)), this, SLOT(moveRemoteItems(QPoint, int)));
	connect(m_network, SIGNAL(deleteRemoteItems(QList<int>)), this, SLOT(deleteRemoteItems(QList<int>)));
	connect(m_network, SIGNAL(editableAuthority()), this, SLOT(editableAuthority()));	

	m_network->connectServer("10.1.0.10", "10001");

	QVBoxLayout* layout = (QVBoxLayout*)ui.widPaint->layout();
	m_view = new STWBView(m_network);
	layout->addWidget(m_view);
	m_view->installEventFilter(this);

	m_vtoolbar = new STWBVToolbar(this);
	int toolbarX = 10;
	int toolbarY = geometry().height() / 2 - m_vtoolbar->height() / 2;
	m_vtoolbar->show();
	m_vtoolbar->move(QPoint(toolbarX, toolbarY));

	m_penStylePanel = new STWBPenStylePanel(this);
	m_penStylePanel->hide();
	m_penStylePanel->move(QPoint(toolbarX + m_vtoolbar->width(), toolbarY));

	m_textStylePanel = new STWBTextStylePanel(this);
	m_textStylePanel->hide();
	m_textStylePanel->move(QPoint(toolbarX + m_vtoolbar->width(), toolbarY + 52));

	connect(m_penStylePanel, SIGNAL(updatePenThickness(int)), this, SLOT(setPenThickness(int)));
	connect(m_penStylePanel, SIGNAL(updatePenColor(QString)), this, SLOT(setPenColor(QString)));
	connect(m_textStylePanel, SIGNAL(updateTextSize(int)), this, SLOT(setTextSize(int)));
	connect(m_textStylePanel, SIGNAL(updateTextColor(QString)), this, SLOT(setTextColor(QString)));
	connect(m_vtoolbar, SIGNAL(setActionMode(int)), this, SLOT(setActionMode(int)));
	connect(m_vtoolbar, SIGNAL(deleteAction()), this, SLOT(deleteAction()));
	connect(m_vtoolbar, SIGNAL(hideStylePanels()), this, SLOT(hideStylePanels()));
	connect(m_vtoolbar, SIGNAL(showPenStylePanel()), this, SLOT(showPenStylePanel()));
	connect(m_vtoolbar, SIGNAL(showTextStylePanel()), this, SLOT(showTextStylePanel()));
	connect(m_vtoolbar, SIGNAL(openCloudFile()), this, SLOT(openCloudFile()));

	m_penStylePanel->init();
	m_textStylePanel->init();
	m_vtoolbar->init();

	on_pbMaximum_clicked();

	m_docWindowIndex = 0;
}

STWhiteBoard::~STWhiteBoard()
{
}

void STWhiteBoard::on_pbMinimum_clicked()
{
	this->window()->showMinimized();
}

void STWhiteBoard::on_pbMaximum_clicked()
{
	ui.pbNormal->setVisible(true);
	ui.pbNormal->setAttribute(Qt::WA_UnderMouse, false);
	ui.pbMaximum->setVisible(false);
	this->window()->showMaximized();
}

void STWhiteBoard::on_pbNormal_clicked()
{
	ui.pbNormal->setVisible(false);
	ui.pbMaximum->setVisible(true);
	ui.pbMaximum->setAttribute(Qt::WA_UnderMouse, false);
	this->window()->showNormal();
}

void STWhiteBoard::on_pbClose_clicked()
{
	m_network->disconnectServer();
	logout();
	m_docWindows.clear();
	close();
}

////////////////////////////////////////////////////////////////////////////////////////////////
void STWhiteBoard::initConferenceClient()
{
	m_hardware_accelerated = false;

	// get resolution
	QRect rec = QApplication::desktop()->screenGeometry();
	m_screen_height = rec.height();
	m_screen_width = rec.width();

	// ice server
	woogeen::conference::IceServer ice;
	ice.urls.push_back("stun:61.152.239.56");
	ice.username = ice.password = "";
	vector<woogeen::conference::IceServer> ice_servers;
	ice_servers.push_back(ice);

	GlobalConfiguration::SetVideoHardwareAccelerationEnabled(m_hardware_accelerated);
	//GlobalConfiguration::SetEncodedVideoFrameEnabled(false);

	// configuration
	ConferenceClientConfiguration config = ConferenceClientConfiguration();
	//config.ice_servers = ice_servers;
	MediaCodec mc;
	mc.video_codec = MediaCodec::VP8;
	//mc.video_codec = MediaCodec::H264;
	config.media_codec = mc;
	//config.max_video_bandwidth = 1000;

	m_client = std::make_shared<ConferenceClient>(config);
	m_client->AddObserver(*this);
}

void STWhiteBoard::newStreamSlot(QString id, int width, int height)
{
	subscribeStream(id);
}

void STWhiteBoard::attachRenderSlot(QString id, std::shared_ptr<RemoteStream> stream)
{
	int renderID = -1;
	for (size_t i = 0; i < MAX_STREAM_NUM; i++)
	{
		if (!m_videoItems[i]->isusing())
		{
			renderID = i;
			m_videoItems[i]->use();
			m_videoItems[i]->setVisible(true);
			break;
		}
	}
	stream->AttachVideoRenderer(m_videoItems[renderID]->getRenderWindow());

	m_all_stream_map[id].isShowing = true;
	m_all_stream_map[id].renderID = renderID;
	ui.widVideo->update();
}

void STWhiteBoard::detachRenderSlot(QString id)
{
	m_all_stream_map[id].stream->DetachVideoRenderer();
	m_all_stream_map[id].isShowing = false;
	m_videoItems[m_all_stream_map[id].renderID]->unuse();
	m_videoItems[m_all_stream_map[id].renderID]->setVisible(false);
	m_all_stream_map[id].renderID = -1;
	ui.widVideo->update();
}

void STWhiteBoard::subscribeStream(QString id)
{
	if (!m_all_stream_map.contains(id))
	{
		return;
	}

	shared_ptr<RemoteStream> mainStream = m_all_stream_map[id].stream;

	/*SubscribeOptions options;
	options.resolution.width = m_all_stream_map[id].width;
	options.resolution.height = m_all_stream_map[id].height;
	//options.video_quality_level = woogeen::conference::SubscribeOptions::VideoQualityLevel::kBestSpeed;*/
	m_client->Subscribe(mainStream,
		//options,
		[&](std::shared_ptr<RemoteStream> stream)
	{
		string streamID = stream->Id();
		Q_EMIT attachRenderSignal(QString::fromStdString(streamID), stream);
		TAHITI_INFO("���ĳɹ�..." << streamID.c_str());
	},
		[=](std::unique_ptr<ConferenceException>)
	{
		TAHITI_ERROR("����ʧ��...");
	});
}

void STWhiteBoard::unsubscribeStream(QString id)
{
	if (!m_all_stream_map.contains(id))
	{
		return;
	}
	m_curentID = id; // TODO:logoutʱ������ɾ�������׳�ͻ,��ʱsleep
	Q_EMIT detachRenderSignal(m_curentID);
	std::shared_ptr<RemoteStream> stream = m_all_stream_map[m_curentID].stream;
	m_client->Unsubscribe(stream,
		[=]()
	{
		TAHITI_INFO("ȡ�����ĳɹ�...");
	},
		[=](std::unique_ptr<ConferenceException>)
	{
		TAHITI_ERROR("ȡ������ʧ��...");
	});
	QThread::msleep(500);
}

void STWhiteBoard::OnStreamAdded(shared_ptr<RemoteCameraStream> stream)
{
	addStream(stream);
}

void STWhiteBoard::OnStreamAdded(shared_ptr<RemoteMixedStream> stream)
{
}

void STWhiteBoard::OnStreamAdded(shared_ptr<RemoteScreenStream> stream)
{
}

void STWhiteBoard::addStream(shared_ptr<RemoteStream> stream, int width, int height)
{
	string id = stream->Id();
	StreamInfo info;
	info.stream = stream;
	info.width = width;
	info.height = height;
	m_all_stream_map.insert(QString::fromStdString(id), info);

	Q_EMIT newStreamSignal(QString::fromStdString(id), width, height);
}

void STWhiteBoard::OnStreamRemoved(std::shared_ptr<RemoteCameraStream> stream)
{
	removeStream(stream);
}

void STWhiteBoard::OnStreamRemoved(std::shared_ptr<RemoteMixedStream> stream)
{
	removeStream(stream);
}

void STWhiteBoard::OnStreamRemoved(std::shared_ptr<RemoteScreenStream> stream)
{
	removeStream(stream);
}

void STWhiteBoard::removeStream(std::shared_ptr<RemoteStream> stream)
{
	string id = stream->Id();
	unsubscribeStream(QString::fromStdString(id));
}

void STWhiteBoard::OnUserJoined(std::shared_ptr<const User> user)
{

}

void STWhiteBoard::OnUserLeft(std::shared_ptr<const User> user)
{

}

void STWhiteBoard::OnServerDisconnected()
{
	//ui.lblStatus->setText(QString::fromLocal8Bit("�������Ͽ�..."));
}

void STWhiteBoard::login()
{
	bool isTeacher = true;
	QString server = STConfig::getConfig("/xmpp/server");
	QString uri = QString("http://") + server + QString(":3001/createToken");

	string token = getToken(string((const char *)uri.toLocal8Bit()), isTeacher);
	if (token != "")
	{
		m_client->Join(token,
			[=](std::shared_ptr<User> user)
		{
			TAHITI_INFO("����ɹ�...");
			sendLocalCamera();
		},
			[=](std::unique_ptr<ConferenceException> err)
		{
			TAHITI_ERROR("����ʧ��...");
		});
	}
	else
	{  // error occurs
	}
}

void STWhiteBoard::logout()
{
	QList<QString> ids = m_all_stream_map.keys();

	QList<QString>::Iterator it;
	for (it = ids.begin(); it != ids.end(); it++)
	{
		unsubscribeStream(*it);
	}
	m_all_stream_map.clear();

	stopSendLocalCamera();

	m_client->Leave(
		[=]
	{
		TAHITI_INFO("�ǳ��ɹ�...");
	},
		[=](std::unique_ptr<woogeen::conference::ConferenceException> err)
	{
		TAHITI_ERROR("�ǳ�ʧ��...");
	});
}

void STWhiteBoard::sendLocalCamera()
{
	int err_code;
	if (!m_local_camera_stream.get())
	{
		m_local_camera_stream_param.reset(new LocalCameraStreamParameters(true, true));
		m_local_camera_stream_param->Resolution(432, 240);
		m_local_camera_stream_param->CameraId(DeviceUtils::VideoCapturerIds()[0]);
		m_local_camera_stream = LocalCameraStream::Create(*m_local_camera_stream_param, err_code);
	}
	m_client->Publish(m_local_camera_stream,
		[=]
	{
		TAHITI_INFO("���ͱ�����Ƶ�ɹ�...");
	},
		[=](std::unique_ptr<ConferenceException> err)
	{
		TAHITI_ERROR("���ͱ�����Ƶʧ��...");
	});
}

void STWhiteBoard::stopSendLocalCamera()
{
	m_client->Unpublish(m_local_camera_stream,
		[=]
	{
		TAHITI_INFO("ֹͣ���ͱ�����Ƶ�ɹ�...");
		m_local_camera_stream_param = nullptr;
		m_local_camera_stream = nullptr;
	},
		[=](std::unique_ptr<ConferenceException> err)
	{
		TAHITI_ERROR("ֹͣ���ͱ�����Ƶʧ��...");
	});
}

////////////////////////////////////////////////////////////////////////////////////////////////
void STWhiteBoard::connectNetworkServer()
{
	m_network->createClient("teacher");
	m_network->createCourse("course-1");
	m_network->joinCourse("course-1");
}

void STWhiteBoard::hideStylePanels()
{
	m_penStylePanel->hide();
	m_textStylePanel->hide();
}

void STWhiteBoard::showPenStylePanel()
{
	m_penStylePanel->show();
	m_textStylePanel->hide();
}

void STWhiteBoard::showTextStylePanel()
{
	m_penStylePanel->hide();
	m_textStylePanel->show();
}

void STWhiteBoard::setActionMode(int mode)
{
	m_action_mode = mode;
	m_view->clearSelection();
	m_view->setMode((ActionType)mode);
	if (mode == ActionType::AT_Select)
	{
		m_view->setDragMode(QGraphicsView::RubberBandDrag);
	}
	else
	{
		m_view->setDragMode(QGraphicsView::NoDrag);
	}
	Q_EMIT setActionModeSignal(mode);
}

void STWhiteBoard::deleteAction()
{
	m_view->deleteSelectedItem();
	Q_EMIT deleteActionSignal();
}

void STWhiteBoard::openCloudFile()
{
	STWBDocWindow* docWindow = new STWBDocWindow(m_network, m_docWindowIndex++, m_view);
	connect(docWindow, SIGNAL(closeCloudFile(int)), this, SLOT(closeCloudFile(int)));
	connect(this, SIGNAL(setPenThicknessSignal(int)), docWindow, SLOT(setPenThickness(int)));
	connect(this, SIGNAL(setPenColorSignal(QString)), docWindow, SLOT(setPenColor(QString)));
	connect(this, SIGNAL(setTextSizeSignal(int)), docWindow, SLOT(setTextSize(int)));
	connect(this, SIGNAL(setTextColorSignal(QString)), docWindow, SLOT(setTextColor(QString)));
	connect(this, SIGNAL(setActionModeSignal(int)), docWindow, SLOT(setActionMode(int)));
	connect(this, SIGNAL(deleteActionSignal()), docWindow, SLOT(deleteAction()));
	Q_EMIT setActionModeSignal(m_action_mode);
	Q_EMIT setPenThicknessSignal(m_pen_thickness);
	Q_EMIT setPenColorSignal(m_pen_color);
	Q_EMIT setTextSizeSignal(m_text_size);
	Q_EMIT setTextColorSignal(m_text_color);

	m_docWindows.append(docWindow);
	int x = 50 * (m_docWindowIndex);
	int y = 35 * (m_docWindowIndex);
	docWindow->show();
	docWindow->move(QPoint(x, y));
}

void STWhiteBoard::closeCloudFile(int index)
{
	QVector<STWBDocWindow*> ::Iterator it;
	for (it = m_docWindows.begin(); it != m_docWindows.end(); it++)
	{
		if ((*it)->getDocWindowIndex() == index)
		{
			m_docWindows.removeOne(*it);
			return;
		}
	}
}

void STWhiteBoard::setPenThickness(int thickness)
{
	m_pen_thickness = thickness;
	m_view->setPenThickness(thickness);
	hideStylePanels();
	if (m_vtoolbar->getCurrentSelect() != 2)
	{
		m_vtoolbar->on_pbPen_clicked();
	}
	Q_EMIT setPenThicknessSignal(thickness);
}

void STWhiteBoard::setPenColor(QString color)
{
	m_pen_color = color;
	m_view->setPenColor(color);
	m_vtoolbar->changePenShowColor(color);
	hideStylePanels();
	if (m_vtoolbar->getCurrentSelect() != 2)
	{
		m_vtoolbar->on_pbPen_clicked();
	}
	Q_EMIT setPenColorSignal(color);
}

void STWhiteBoard::setTextSize(int size)
{
	m_text_size = size;
	m_view->setTextSize(size);
	hideStylePanels();
	if (m_vtoolbar->getCurrentSelect() != 3)
	{
		m_vtoolbar->on_pbText_clicked();
	}
	Q_EMIT setTextSizeSignal(size);
}

void STWhiteBoard::setTextColor(QString color)
{
	m_text_color = color;
	m_view->setTextColor(color);
	m_vtoolbar->changeTextShowColor(color);
	hideStylePanels();
	if (m_vtoolbar->getCurrentSelect() != 3)
	{
		m_vtoolbar->on_pbText_clicked();
	}
	Q_EMIT setTextColorSignal(color);
}

void STWhiteBoard::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}
}

void STWhiteBoard::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint pos = this->pos() + movePoint;
		m_startMovePos = event->globalPos();
		this->move(pos.x(), pos.y());
	}
}

void STWhiteBoard::mouseReleaseEvent(QMouseEvent* event)
{
	m_isPressed = false;
}

bool STWhiteBoard::eventFilter(QObject* watched, QEvent* e)
{
	if (watched == m_view && e->type() == QEvent::Enter)
	{
		hideStylePanels();
	}
	return QWidget::eventFilter(watched, e);
}

void STWhiteBoard::resizeEvent(QResizeEvent* size)
{
	int toolbarX = 10;
	int toolbarY = geometry().height() / 2 - m_vtoolbar->height() / 2;
	m_vtoolbar->show();
	m_vtoolbar->move(QPoint(toolbarX, toolbarY));
	m_penStylePanel->move(QPoint(toolbarX + m_vtoolbar->width(), toolbarY));
	m_textStylePanel->move(QPoint(toolbarX + m_vtoolbar->width(), toolbarY + 52));
}

void STWhiteBoard::drawRemoteRealtimePen(QString color, int thickness, QVector<QPoint> points)
{
	setPenColor(color);
	setPenThickness(thickness);
	m_view->drawRemoteRealtimePen(color, thickness, points);
}

void STWhiteBoard::drawRemotePenItem(QString color, int thickness, QVector<QPoint> points, int itemID)
{
	setPenColor(color);
	setPenThickness(thickness);
	m_view->drawRemotePenItem(color, thickness, points, itemID);
}

void STWhiteBoard::drawRemoteTextItem(QString color, int size, QString content, QPoint pos, int itemID)
{
	setTextColor(color);
	setTextSize(size);
	m_view->drawRemoteTextItem(color, size, content, pos, itemID);
}

void STWhiteBoard::moveRemoteItems(QPoint pos, int itemID)
{
	m_view->moveRemoteItems(pos, itemID);
}

void STWhiteBoard::deleteRemoteItems(QList<int> itemIDs)
{
	m_view->deleteRemoteItems(itemIDs);
}

void STWhiteBoard::editableAuthority(QString editable)
{
	m_vtoolbar->init();
	if (editable == "True")
	{
		m_penStylePanel->show();
		m_textStylePanel->show();
		m_vtoolbar->show();
	}
	else
	{
		m_penStylePanel->hide();
		m_textStylePanel->hide();
		m_vtoolbar->hide();
	}
}