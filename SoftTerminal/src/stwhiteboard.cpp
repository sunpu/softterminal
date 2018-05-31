#include "stwhiteboard.h"

using namespace tahiti;

STWhiteBoard::STWhiteBoard(QString jid, QString name, QWidget *parent)
	: m_jid(jid), m_name(name), QWidget(parent)
{
	ui.setupUi(this);

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

	// 视频相关
	initConferenceClient();
	login();

	QObject::connect(this, SIGNAL(newStreamSignal(QString, int, int)), this, SLOT(newStreamSlot(QString, int, int)));
	QObject::connect(this, SIGNAL(showLocalCameraSignal()), this, SLOT(showLocalCamera()));
	QObject::connect(this, SIGNAL(unshowLocalCameraSignal()), this, SLOT(unshowLocalCamera()));

	qRegisterMetaType<std::shared_ptr<RemoteStream>>("std::shared_ptr<RemoteStream>");
	QObject::connect(this, SIGNAL(attachRenderSignal(QString, std::shared_ptr<RemoteStream>)), this, SLOT(attachRenderSlot(QString, std::shared_ptr<RemoteStream>)));
	QObject::connect(this, SIGNAL(detachRenderSignal(QString)), this, SLOT(detachRenderSlot(QString)));

	QHBoxLayout* hboxLayout = (QHBoxLayout*)ui.widVideo->layout();
	QGridLayout* gridLayout = (QGridLayout*)ui.widVideoBig->layout();
	m_videoItems.resize(MAX_STREAM_NUM);
	m_big_videoItems.resize(MAX_STREAM_NUM);
	for (size_t i = 0; i < MAX_STREAM_NUM; i++)
	{
		m_videoItems[i] = new STWBVideoItem(ui.widVideo);
		QObject::connect(m_videoItems[i], SIGNAL(muteSignal(QString)), this, SLOT(mute(QString)));
		QObject::connect(m_videoItems[i], SIGNAL(unmuteSignal(QString)), this, SLOT(unmute(QString)));
		QObject::connect(this, SIGNAL(muteResultSignal(bool, QString)), m_videoItems[i], SLOT(onMuteResult(bool, QString)));
		QObject::connect(this, SIGNAL(unmuteResultSignal(bool, QString)), m_videoItems[i], SLOT(onUnmuteResult(bool, QString)));
		m_videoItems[i]->setRenderSize(192, 108);
		m_videoItems[i]->setBgImage(":/SoftTerminal/images/video_bg.png");
		m_videoItems[i]->setObjectName(QString::number(i));
		m_videoItems[i]->setVisible(false);
		hboxLayout->addWidget(m_videoItems[i]);

		m_big_videoItems[i] = new STWBVideoItem(ui.widVideoBig);
		QObject::connect(m_big_videoItems[i], SIGNAL(muteSignal(QString)), this, SLOT(mute(QString)));
		QObject::connect(m_big_videoItems[i], SIGNAL(unmuteSignal(QString)), this, SLOT(unmute(QString)));
		QObject::connect(this, SIGNAL(muteResultSignal(bool, QString)), m_big_videoItems[i], SLOT(onMuteResult(bool, QString)));
		QObject::connect(this, SIGNAL(unmuteResultSignal(bool, QString)), m_big_videoItems[i], SLOT(onUnmuteResult(bool, QString)));
		m_big_videoItems[i]->setRenderSize(400, 225);
		m_big_videoItems[i]->setBgImage(":/SoftTerminal/images/video_bg_big.png");
		m_big_videoItems[i]->setObjectName(QString::number(i));
		m_big_videoItems[i]->setVisible(false);
		gridLayout->addWidget(m_big_videoItems[i], i / 3, i % 3);
	}

	m_localCameraRenderID = -1;

	// 白板相关
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

	QString server = STConfig::getConfig("/xmpp/server");
	m_network->connectServer(server, "10001");

	QVBoxLayout* layout = (QVBoxLayout*)ui.widPaint->layout();
	m_view = new STWBView(m_network);
	layout->addWidget(m_view);
	m_view->installEventFilter(this);
	QStackedLayout* stackLayout = new QStackedLayout;
	stackLayout->setStackingMode(QStackedLayout::StackAll);
	m_view->setLayout(stackLayout);

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
	connect(m_vtoolbar, SIGNAL(openCloudFileSignal(QString)), this, SLOT(openCloudFile(QString)));

	m_penStylePanel->init();
	m_textStylePanel->init();
	m_vtoolbar->init();

	m_docWindowIndex = 0;

	// 页面初始化
	ui.pbNormal->setVisible(false);
	m_vtoolbar->hide();
	m_currentIndex = 1;
	ui.swMain->setCurrentIndex(m_currentIndex);
	ui.widTitle->installEventFilter(this);

	on_pbMaximum_clicked();
}

STWhiteBoard::~STWhiteBoard()
{
}

void STWhiteBoard::switchShowMode(bool mode)
{
	QList<QString> ids = m_all_stream_map.keys();
	QList<QString>::Iterator it;
	int renderID = -1;

	if (!mode)
	{
		m_currentIndex = 1;
		ui.swMain->setCurrentIndex(m_currentIndex);
		m_vtoolbar->hide();

		for (it = ids.begin(); it != ids.end(); it++)
		{
			renderID = m_all_stream_map[*it].renderID;
			if (m_all_stream_map[*it].stream.get())
			{
				m_all_stream_map[*it].stream->DetachVideoRenderer();
			}
			m_videoItems[renderID]->unuse();
			m_videoItems[renderID]->setVisible(false);

			m_big_videoItems[renderID]->use(*it, m_all_stream_map[*it].showName, m_all_stream_map[*it].mute);
			m_big_videoItems[renderID]->setVisible(true);
			m_all_stream_map[*it].stream->AttachVideoRenderer(m_big_videoItems[renderID]->getRenderWindow());
		}

		if (m_localCameraRenderID != -1)
		{
			if (m_local_camera_stream.get())
			{
				m_local_camera_stream->DetachVideoRenderer();
			}
			m_videoItems[m_localCameraRenderID]->unuse();
			m_videoItems[m_localCameraRenderID]->setVisible(false);

			m_big_videoItems[m_localCameraRenderID]->use("", QStringLiteral("我"), false);
			m_big_videoItems[m_localCameraRenderID]->setVisible(true);
			m_local_camera_stream->AttachVideoRenderer(m_big_videoItems[m_localCameraRenderID]->getRenderWindow());
		}
	}
	else
	{
		m_currentIndex = 0;
		ui.swMain->setCurrentIndex(m_currentIndex);
		m_vtoolbar->show();

		for (it = ids.begin(); it != ids.end(); it++)
		{
			renderID = m_all_stream_map[*it].renderID;
			if (m_all_stream_map[*it].stream.get())
			{
				m_all_stream_map[*it].stream->DetachVideoRenderer();
			}
			m_big_videoItems[renderID]->unuse();
			m_big_videoItems[renderID]->setVisible(false);

			m_videoItems[renderID]->use(*it, m_all_stream_map[*it].showName, m_all_stream_map[*it].mute);
			m_videoItems[renderID]->setVisible(true);
			m_all_stream_map[*it].stream->AttachVideoRenderer(m_videoItems[renderID]->getRenderWindow());
		}

		if (m_localCameraRenderID != -1)
		{
			if (m_local_camera_stream.get())
			{
				m_local_camera_stream->DetachVideoRenderer();
			}
			m_big_videoItems[m_localCameraRenderID]->unuse();
			m_big_videoItems[m_localCameraRenderID]->setVisible(false);

			m_videoItems[m_localCameraRenderID]->use("", QStringLiteral("我"), false);
			m_videoItems[m_localCameraRenderID]->setVisible(true);
			m_local_camera_stream->AttachVideoRenderer(m_videoItems[m_localCameraRenderID]->getRenderWindow());
		}
	}
	ui.widVideo->update();
	ui.widVideoBig->update();
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

void STWhiteBoard::resizeMaximumDocWindow()
{
	QVector<STWBDocWindow*> ::Iterator it;
	QSize size = m_view->size();
	for (it = m_docWindows.begin(); it != m_docWindows.end(); it++)
	{
		if ((*it)->isMaximum())
		{
			(*it)->on_pbMaximum_clicked();
		}
	}
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
	ics::conference::IceServer ice;
	ice.urls.push_back("stun:221.226.156.109:3478");
	ice.username = ice.password = "";
	vector<ics::conference::IceServer> ice_servers;
	ice_servers.push_back(ice);

	GlobalConfiguration::SetVideoHardwareAccelerationEnabled(m_hardware_accelerated);
	//GlobalConfiguration::SetEncodedVideoFrameEnabled(false);

	// configuration
	ConferenceClientConfiguration config = ConferenceClientConfiguration();
	//config.ice_servers = ice_servers;

	m_client = ConferenceClient::Create(config);
	m_client->AddObserver(*this);
}

void STWhiteBoard::newStreamSlot(QString id, int width, int height)
{
	subscribeStream(id);
}

void STWhiteBoard::attachRenderSlot(QString id, std::shared_ptr<RemoteStream> stream)
{
	stream = m_all_stream_map[id].stream;
	int renderID = -1;
	if (m_currentIndex == 0)
	{
		for (size_t i = 0; i < MAX_STREAM_NUM; i++)
		{
			if (!m_videoItems[i]->isusing())
			{
				renderID = i;
				m_videoItems[i]->use(id, m_all_stream_map[id].showName, m_all_stream_map[id].mute);
				m_videoItems[i]->setVisible(true);
				break;
			}
		}
		if (renderID == -1)
		{
			return;
		}
		stream->AttachVideoRenderer(m_videoItems[renderID]->getRenderWindow());
		ui.widVideo->update();
	}
	else
	{
		for (size_t i = 0; i < MAX_STREAM_NUM; i++)
		{
			if (!m_big_videoItems[i]->isusing())
			{
				renderID = i;
				m_big_videoItems[i]->use(id, m_all_stream_map[id].showName, m_all_stream_map[id].mute);
				m_big_videoItems[i]->setVisible(true);
				break;
			}
		}
		if (renderID == -1)
		{
			return;
		}
		stream->AttachVideoRenderer(m_big_videoItems[renderID]->getRenderWindow());
		ui.widVideoBig->update();
	}
	m_all_stream_map[id].isShowing = true;
	m_all_stream_map[id].renderID = renderID;
}

void STWhiteBoard::detachRenderSlot(QString id)
{
	if (m_all_stream_map[id].stream)
	{
		m_all_stream_map[id].stream->DetachVideoRenderer();
	}
	m_all_stream_map[id].isShowing = false;
	if (m_all_stream_map[id].renderID == -1)
	{
		return;
	}
	if (m_currentIndex == 0)
	{
		m_videoItems[m_all_stream_map[id].renderID]->unuse();
		m_videoItems[m_all_stream_map[id].renderID]->setVisible(false);
		ui.widVideo->update();
	}
	else
	{
		m_big_videoItems[m_all_stream_map[id].renderID]->unuse();
		m_big_videoItems[m_all_stream_map[id].renderID]->setVisible(false);
		ui.widVideoBig->update();
	}
	m_all_stream_map[id].renderID = -1;
}

void STWhiteBoard::mute(QString id)
{
	m_all_stream_map[id].subscription->Mute(TrackKind::kAudio,
		[=]()
	{
		m_all_stream_map[id].mute = true;
		Q_EMIT muteResultSignal(true, id);
	},
		[=](std::unique_ptr<Exception>)
	{
		Q_EMIT muteResultSignal(false, id);
	});
}

void STWhiteBoard::unmute(QString id)
{
	m_all_stream_map[id].subscription->Unmute(TrackKind::kAudio,
		[=]()
	{
		m_all_stream_map[id].mute = false;
		Q_EMIT unmuteResultSignal(true, id);
	},
		[=](std::unique_ptr<Exception>)
	{
		Q_EMIT unmuteResultSignal(false, id);
	});
}

void STWhiteBoard::subscribeStream(QString id)
{
	if (!m_all_stream_map.contains(id))
	{
		return;
	}

	shared_ptr<RemoteStream> mainStream = m_all_stream_map[id].stream;

	SubscribeOptions options;
	VideoCodecParameters codec_params;
	codec_params.name = ics::base::VideoCodec::kH264;
	options.video.codecs.push_back(codec_params);
	//options.video.resolution.width = 1920;
	//options.video.resolution.height = 1080;
	AudioCodecParameters audio_params;
	audio_params.name = ics::base::AudioCodec::kOpus;
	options.audio.codecs.push_back(audio_params);
	m_client->Subscribe(mainStream,
		options,
		[&](std::shared_ptr<ConferenceSubscription> subscription)
	{
		string streamID = subscription->StreamId();
		m_all_stream_map[streamID.c_str()].subscription = subscription;
		ConfSubObserver* subObserver = new ConfSubObserver(streamID.c_str());
		connect(subObserver, SIGNAL(subscriptionEndSignal(QString)), this, SLOT(subscriptionEnd(QString)));
		subscription->AddObserver(*subObserver);
		Q_EMIT attachRenderSignal(QString::fromStdString(streamID), NULL);
		TAHITI_INFO("订阅成功..." << streamID.c_str());
	},
		[=](std::unique_ptr<Exception>)
	{
		TAHITI_ERROR("订阅失败...");
	});
}

void STWhiteBoard::unsubscribeStream(QString id)
{
	if (!m_all_stream_map.contains(id))
	{
		return;
	}
	if (m_all_stream_map[id].stream.get())
	{
		//m_curentID = id; // logout时，批量删除，容易冲突,暂时sleep
		//Q_EMIT detachRenderSignal(m_curentID);
		Q_EMIT detachRenderSignal(id);
		//m_all_stream_map[id].subscription->Stop();
	}
	//QThread::msleep(500);
}

void STWhiteBoard::OnStreamAdded(std::shared_ptr<ics::conference::RemoteStream> stream)
{
	unordered_map<string, string> attributes = stream->Attributes();
	if (attributes.find("jid") != attributes.end())
	{
		if (m_jid == attributes["jid"].c_str())
		{
			return;
		}
	}
	if (stream->Source().video == VideoSourceInfo::kCamera)
	{
		addStream(stream);
	}
}

void STWhiteBoard::addStream(shared_ptr<RemoteStream> stream, int width, int height)
{
	string id = stream->Id();
	StreamInfo info;
	info.stream = stream;
	info.width = width;
	info.height = height;
	unordered_map<string, string> attributes = stream->Attributes();
	if (attributes.find("name") != attributes.end())
	{
		info.showName = attributes["name"].c_str();
	}
	m_all_stream_map.insert(QString::fromStdString(id), info);

	Q_EMIT newStreamSignal(QString::fromStdString(id), width, height);
}

void STWhiteBoard::OnStreamRemoved(std::shared_ptr<ics::conference::RemoteStream> stream)
{
	removeStream(stream);
}

void STWhiteBoard::removeStream(std::shared_ptr<RemoteStream> stream)
{
	string id = stream->Id();
	unsubscribeStream(QString::fromStdString(id));
	m_all_stream_map.remove(QString::fromStdString(id));
}

void STWhiteBoard::subscriptionEnd(QString id)
{
	unsubscribeStream(id);
	m_all_stream_map[id].subscription->Stop();
	m_all_stream_map.remove(id);
}

void STWhiteBoard::OnServerDisconnected()
{
	//ui.lblStatus->setText(QString::fromLocal8Bit("服务器断开..."));
}

void STWhiteBoard::login()
{
	bool isTeacher = true;
	QString server = STConfig::getConfig("/xmpp/server");
	QString uri = QString("http://") + server + QString(":3001/CreateToken");

	string token = getToken(string((const char *)uri.toLocal8Bit()), isTeacher);
	if (token != "")
	{
		std::lock_guard<std::mutex> lock(m_callback_mutex);
		m_client->Join(token,
			[=](std::shared_ptr<ConferenceInfo> info)
		{
			std::vector<std::shared_ptr<RemoteStream>> remote_streams = info->RemoteStreams();
			for (auto& remote_stream : remote_streams)
			{
				if (remote_stream->Source().video == VideoSourceInfo::kCamera)
				{
					addStream(remote_stream);
				}
			}
			TAHITI_INFO("登入成功...");
			sendLocalCamera();
		},
			[=](unique_ptr<Exception> err)
		{
			TAHITI_ERROR("登入失败...");
		}
		);
	}
	else
	{
		// error occurs
		cout << "Create token error!" << endl;
	}
}

void STWhiteBoard::logout()
{
	if (m_localCameraRenderID != -1)
	{
		Q_EMIT unshowLocalCameraSignal();
	}

	QList<QString> ids = m_all_stream_map.keys();

	QList<QString>::Iterator it;
	for (it = ids.begin(); it != ids.end(); it++)
	{
		unsubscribeStream(*it);
	}
	m_all_stream_map.clear();

	stopSendLocalCamera();
}

void STWhiteBoard::showLocalCamera()
{
	int renderID = -1;
	if (m_currentIndex == 0)
	{
		for (size_t i = 0; i < MAX_STREAM_NUM; i++)
		{
			if (!m_videoItems[i]->isusing())
			{
				renderID = i;
				m_videoItems[i]->use("", QStringLiteral("我"), false);
				m_videoItems[i]->setVisible(true);
				break;
			}
		}
		if (renderID == -1)
		{
			return;
		}
		m_local_camera_stream->AttachVideoRenderer(m_videoItems[renderID]->getRenderWindow());
		ui.widVideo->update();
	}
	else
	{
		for (size_t i = 0; i < MAX_STREAM_NUM; i++)
		{
			if (!m_big_videoItems[i]->isusing())
			{
				renderID = i;
				m_big_videoItems[i]->use("", QStringLiteral("我"), false);
				m_big_videoItems[i]->setVisible(true);
				break;
			}
		}
		if (renderID == -1)
		{
			return;
		}
		m_local_camera_stream->AttachVideoRenderer(m_big_videoItems[renderID]->getRenderWindow());
		ui.widVideoBig->update();
	}
	m_localCameraRenderID = renderID;
}

void STWhiteBoard::unshowLocalCamera()
{
	if (m_local_camera_stream.get())
	{
		m_local_camera_stream->DetachVideoRenderer();
	}
	if (m_currentIndex == 0)
	{
		m_videoItems[m_localCameraRenderID]->unuse();
		m_videoItems[m_localCameraRenderID]->setVisible(false);
		ui.widVideo->update();
	}
	else
	{
		m_big_videoItems[m_localCameraRenderID]->unuse();
		m_big_videoItems[m_localCameraRenderID]->setVisible(false);
		ui.widVideoBig->update();
	}
	m_localCameraRenderID = -1;
}

void STWhiteBoard::sendLocalCamera()
{
	int err_code;

	QString cameraId = STConfig::getConfig("/camera/id");
	if (cameraId.size() == 0)
	{
		return;
	}
	std::string capturerId;
	std::vector<std::string> capturerIds = DeviceUtils::VideoCapturerIds();
	for (int i = 0; i < capturerIds.size(); i++)
	{
		if (cameraId.contains(capturerIds[i].c_str()))
		{
			capturerId = capturerIds[i];
			break;
		}
	}
	if (capturerId.size() == 0)
	{
		return;
	}

	int width = STConfig::getConfig("/camera/width").toInt();
	int height = STConfig::getConfig("/camera/height").toInt();
	if (!m_local_camera_stream.get())
	{
		m_local_camera_stream_param.reset(new LocalCameraStreamParameters(true, true));
		m_local_camera_stream_param->Resolution(width, height);
		m_local_camera_stream_param->CameraId(capturerId);
		m_local_camera_stream = LocalStream::Create(*m_local_camera_stream_param, err_code);
		if (!m_local_camera_stream.get())
		{
			return;
		}
	}
	PublishOptions options;
	VideoCodecParameters codec_params;
	codec_params.name = ics::base::VideoCodec::kH264;
	VideoEncodingParameters video_params(codec_params, 0, m_hardware_accelerated);
	options.video.push_back(video_params);

	AudioEncodingParameters audio_params;
	audio_params.codec.name = ics::base::AudioCodec::kOpus;
	options.audio.push_back(audio_params);

	unordered_map<string, string> attributes;
	string jid = m_jid.toUtf8().constData();
	string name = m_name.toUtf8().constData();
	attributes.insert(unordered_map<string, string>::value_type("jid", jid));
	attributes.insert(unordered_map<string, string>::value_type("name", name));
	m_local_camera_stream->Attributes(attributes);

	m_client->Publish(m_local_camera_stream,
		options,
		[=](std::shared_ptr<ConferencePublication> publication)
	{
		m_publication = publication;
		TAHITI_INFO("发送本地视频成功...");
		Q_EMIT showLocalCameraSignal();
	},
		[=](unique_ptr<Exception> err)
	{
		TAHITI_ERROR("发送本地视频失败...");
	});
}

void STWhiteBoard::stopSendLocalCamera()
{
	if (m_publication.get())
	{
		m_publication->Stop();

		m_publication.reset();
		m_publication = nullptr;
		m_local_camera_stream->Close();
		m_local_camera_stream.reset();
		m_local_camera_stream_param = nullptr;
		m_local_camera_stream = nullptr;
		TAHITI_INFO("停止发送本地视频成功...");
	}
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

void STWhiteBoard::openCloudFile(QString path)
{
	STWBDocWindow* docWindow = new STWBDocWindow(m_network, path, m_docWindowIndex++, m_view);
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
	docWindow->show();
}

void STWhiteBoard::closeCloudFile(int index)
{
	QVector<STWBDocWindow*>::Iterator it;
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
	else if (e->type() == QEvent::MouseButtonDblClick && ui.widTitle == watched)
	{
		if (ui.pbNormal->isVisible())
		{
			on_pbNormal_clicked();
		}
		else
		{
			on_pbMaximum_clicked();
		}
	}
	return QWidget::eventFilter(watched, e);
}

void STWhiteBoard::resizeEvent(QResizeEvent* size)
{
	int toolbarX = 10;
	int toolbarY = geometry().height() / 2 - m_vtoolbar->height() / 2;
	//m_vtoolbar->show();
	m_vtoolbar->move(QPoint(toolbarX, toolbarY));
	m_penStylePanel->move(QPoint(toolbarX + m_vtoolbar->width(), toolbarY));
	m_textStylePanel->move(QPoint(toolbarX + m_vtoolbar->width(), toolbarY + 52));

	resizeMaximumDocWindow();
}

void STWhiteBoard::showEvent(QShowEvent* event)
{
	this->setAttribute(Qt::WA_Mapped);
	QWidget::showEvent(event);
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

////////////////////////////////////////////////////////////////////////////////////////////////
ConfSubObserver::ConfSubObserver(QString id)
	: m_id(id)
{
}

ConfSubObserver::~ConfSubObserver()
{
}

void ConfSubObserver::OnEnded()
{
	Q_EMIT subscriptionEndSignal(m_id);
}

void ConfSubObserver::OnMute(ics::base::TrackKind track_kind)
{
	cout << "on video/audio muted" << endl;
}

void ConfSubObserver::OnUnmute(ics::base::TrackKind track_kind)
{
	cout << "on video/audio unmuted" << endl;
}