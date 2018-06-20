#include "stwhiteboard.h"

using namespace tahiti;

STWhiteBoard::STWhiteBoard(XmppClient* client, QWidget *parent)
	: m_xmppClient(client), QWidget(parent)
{
	ui.setupUi(this);

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

	m_messageClient = new STMessageClient;
	connect(m_messageClient, SIGNAL(whiteBoardMessageSignal(QString)),
		this, SLOT(whiteBoardMessageSlot(QString)));

	connect(this, SIGNAL(sendKeepaliveSignal()), this, SLOT(sendKeepaliveSlot()));
	connect(this, SIGNAL(newStreamSignal(QString, int, int)), this, SLOT(newStreamSlot(QString, int, int)));
	connect(this, SIGNAL(showLocalCameraSignal()), this, SLOT(showLocalCamera()));
	connect(this, SIGNAL(unshowLocalCameraSignal()), this, SLOT(unshowLocalCamera()));

	qRegisterMetaType<std::shared_ptr<RemoteStream>>("std::shared_ptr<RemoteStream>");
	connect(this, SIGNAL(attachRenderSignal(QString, std::shared_ptr<RemoteStream>)), this, SLOT(attachRenderSlot(QString, std::shared_ptr<RemoteStream>)));
	connect(this, SIGNAL(detachRenderSignal(QString)), this, SLOT(detachRenderSlot(QString)));

	QHBoxLayout* hboxLayout = (QHBoxLayout*)ui.widVideo->layout();
	QGridLayout* gridLayout = (QGridLayout*)ui.widVideoBig->layout();
	m_videoItems.resize(MAX_STREAM_NUM);
	m_big_videoItems.resize(MAX_STREAM_NUM);
	for (size_t i = 0; i < MAX_STREAM_NUM; i++)
	{
		m_videoItems[i] = new STWBVideoItem(ui.widVideo);
		connect(m_videoItems[i], SIGNAL(muteSignal(QString)), this, SLOT(mute(QString)));
		connect(m_videoItems[i], SIGNAL(unmuteSignal(QString)), this, SLOT(unmute(QString)));
		connect(this, SIGNAL(muteResultSignal(bool, QString)), m_videoItems[i], SLOT(onMuteResult(bool, QString)));
		connect(this, SIGNAL(unmuteResultSignal(bool, QString)), m_videoItems[i], SLOT(onUnmuteResult(bool, QString)));
		m_videoItems[i]->setRenderSize(192, 108);
		m_videoItems[i]->setBgImage(":/SoftTerminal/images/video_bg.png");
		m_videoItems[i]->setObjectName(QString::number(i));
		m_videoItems[i]->setVisible(false);
		hboxLayout->addWidget(m_videoItems[i]);

		m_big_videoItems[i] = new STWBVideoItem(ui.widVideoBig);
		connect(m_big_videoItems[i], SIGNAL(muteSignal(QString)), this, SLOT(mute(QString)));
		connect(m_big_videoItems[i], SIGNAL(unmuteSignal(QString)), this, SLOT(unmute(QString)));
		connect(this, SIGNAL(muteResultSignal(bool, QString)), m_big_videoItems[i], SLOT(onMuteResult(bool, QString)));
		connect(this, SIGNAL(unmuteResultSignal(bool, QString)), m_big_videoItems[i], SLOT(onUnmuteResult(bool, QString)));
		m_big_videoItems[i]->setRenderSize(400, 225);
		m_big_videoItems[i]->setBgImage(":/SoftTerminal/images/video_bg_big.png");
		m_big_videoItems[i]->setObjectName(QString::number(i));
		m_big_videoItems[i]->setVisible(false);
		gridLayout->addWidget(m_big_videoItems[i], i / 3, i % 3);
	}

	// 白板相关
	qRegisterMetaType<QVector<QPoint>>("QVector<QPoint>");
	qRegisterMetaType<QPoint>("QPoint");
	qRegisterMetaType<QList<int>>("QList<int>");

	QVBoxLayout* layout = (QVBoxLayout*)ui.widPaint->layout();
	m_view = new STWBView;
	layout->addWidget(m_view);
	m_view->installEventFilter(this);
	QStackedLayout* stackLayout = new QStackedLayout;
	stackLayout->setStackingMode(QStackedLayout::StackAll);
	m_view->setLayout(stackLayout);

	m_raiseHandPanel = new STWBRaiseHandPanel(this);
	int raiseHandPanelX = geometry().width() - m_raiseHandPanel->width() - 10;
	int raiseHandPanelY = geometry().height() - m_raiseHandPanel->height() - 30;
	m_raiseHandPanel->show();
	m_raiseHandPanel->move(QPoint(raiseHandPanelX, raiseHandPanelY));

	m_vtoolbar = new STWBVToolbar(this);
	int toolbarX = geometry().width() - m_vtoolbar->width() - 10;
	int toolbarY = geometry().height() / 2 - m_vtoolbar->height() / 2;
	//m_vtoolbar->show();
	m_vtoolbar->move(QPoint(toolbarX, toolbarY));

	m_penStylePanel = new STWBPenStylePanel(this);
	m_penStylePanel->hide();
	m_penStylePanel->move(QPoint(toolbarX - m_penStylePanel->width(), toolbarY));

	m_textStylePanel = new STWBTextStylePanel(this);
	m_textStylePanel->hide();
	m_textStylePanel->move(QPoint(toolbarX - m_textStylePanel->width(), toolbarY + 52));

	connect(m_penStylePanel, SIGNAL(updatePenThickness(int)), this, SLOT(setPenThickness(int)));
	connect(m_penStylePanel, SIGNAL(updatePenColor(QString)), this, SLOT(setPenColor(QString)));
	connect(m_textStylePanel, SIGNAL(updateTextSize(int)), this, SLOT(setTextSize(int)));
	connect(m_textStylePanel, SIGNAL(updateTextColor(QString)), this, SLOT(setTextColor(QString)));
	connect(m_vtoolbar, SIGNAL(setActionMode(int)), this, SLOT(setActionMode(int)));
	connect(m_vtoolbar, SIGNAL(deleteAction()), this, SLOT(deleteAction()));
	connect(m_vtoolbar, SIGNAL(hideStylePanels()), this, SLOT(hideStylePanels()));
	connect(m_vtoolbar, SIGNAL(showPenStylePanel()), this, SLOT(showPenStylePanel()));
	connect(m_vtoolbar, SIGNAL(showTextStylePanel()), this, SLOT(showTextStylePanel()));
	connect(m_vtoolbar, SIGNAL(openCloudFileViewSignal()), this, SLOT(openCloudFileView()));
	connect(m_vtoolbar, SIGNAL(closeCloudFileViewSignal()), this, SLOT(closeCloudFileView()));
	connect(m_vtoolbar, SIGNAL(openInviteFriendSignal()), this, SLOT(openInviteFriend()));
	connect(m_vtoolbar, SIGNAL(closeInviteFriendSignal()), this, SLOT(closeInviteFriend()));
	connect(m_vtoolbar, SIGNAL(openRosterSignal()), this, SLOT(openRoster()));
	connect(m_vtoolbar, SIGNAL(closeRosterSignal()), this, SLOT(closeRoster()));
	connect(m_vtoolbar, SIGNAL(deleteCourseSignal()), this, SLOT(deleteCourse()));
	connect(this, SIGNAL(toolbarModeSignal(int)), m_vtoolbar, SLOT(toolbarModeSlot(int)));

	m_cloud_file_view = new STWBCloudFileView(this);
	connect(m_cloud_file_view, SIGNAL(closeCloudFileView()), m_vtoolbar, SLOT(closeCloudFileView()));
	connect(m_cloud_file_view, SIGNAL(openCloudFileSignal(QString)), this, SLOT(openCloudFile(QString)));
	m_cloud_file_view->hide();

	m_inviteFriend = new STWBInviteFriend(m_xmppClient, this);
	connect(m_inviteFriend, SIGNAL(closeInviteFriend()), m_vtoolbar, SLOT(closeInviteFriend()));
	m_inviteFriend->hide();

	m_roster = new STWBRoster(this);
	connect(m_roster, SIGNAL(closeRoster()), m_vtoolbar, SLOT(closeRoster()));
	connect(m_roster, SIGNAL(setAuthorityStatusSignal(QString, QString, bool)),
		this, SLOT(setAuthorityStatusSlot(QString, QString, bool)));
	connect(this, SIGNAL(updateAuthorityStatusSignal(QString, QString, bool)),
		m_roster, SLOT(updateAuthorityStatusSlot(QString, QString, bool)));
	connect(this, SIGNAL(refreshRosterSignal()), m_roster, SLOT(refreshRosterTable()));
	m_roster->hide();

	ui.widTitle->installEventFilter(this);
	ui.pgVideo->installEventFilter(this);

	m_localCameraRenderID = -1;
}

STWhiteBoard::~STWhiteBoard()
{
}

void STWhiteBoard::init()
{
	m_show = false;
	m_operate = false;
	m_mic = "disable";
	m_camera = "disable";
	m_group = NULL;
	m_canOperate = false;

	// 页面初始化
	SwitchButton* button = (SwitchButton*)ui.widSwitch;
	button->reset();

	m_localCameraRenderID = -1;

	m_penStylePanel->init();
	m_textStylePanel->init();
	m_vtoolbar->init();

	m_docWindowIndex = 0;

	ui.pbNormal->setVisible(false);
	//m_raiseHandPanel->hide();
	//m_vtoolbar->hide();

	m_currentIndex = 1;
	ui.swMain->setCurrentIndex(m_currentIndex);

	on_pbMaximum_clicked();
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
		//m_raiseHandPanel->hide();
		//m_vtoolbar->hide();

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
			if (m_local_camera_stream.get())
			{
				m_local_camera_stream->AttachVideoRenderer(m_big_videoItems[m_localCameraRenderID]->getRenderWindow());
			}
		}

		if (m_isAdmin)
		{
			Q_EMIT toolbarModeSignal(ToolbarMode::Video_Teacher);
		}
		else
		{
			Q_EMIT toolbarModeSignal(ToolbarMode::Video_Student);
		}
	}
	else
	{
		m_currentIndex = 0;
		ui.swMain->setCurrentIndex(m_currentIndex);
		//m_raiseHandPanel->show();
		//m_vtoolbar->show();

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
			if (m_local_camera_stream.get())
			{
				m_local_camera_stream->AttachVideoRenderer(m_videoItems[m_localCameraRenderID]->getRenderWindow());
			}
		}
		if (m_isAdmin)
		{
			Q_EMIT toolbarModeSignal(ToolbarMode::WhiteBoard_Teacher);
		}
		else
		{
			if (m_canOperate)
			{
				Q_EMIT toolbarModeSignal(ToolbarMode::WhiteBoard_Operator);
			}
			else
			{
				Q_EMIT toolbarModeSignal(ToolbarMode::WhiteBoard_Student);
			}
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
	if (m_admin != m_jid)
	{
		pthread_cancel(m_tidKeepalive);
		void* tmp;
		pthread_join(m_tidKeepalive, &tmp);
		m_group = NULL;
	}

	// {"type":"course","action":"quit","courseID":"111","jid":"a"}
	QString msg = QString("{\"type\":\"course\",\"action\":\"quit\",\"courseID\":\"%1\","
		"\"jid\":\"%2\"}").arg(m_courseID, m_jid);
	m_messageClient->sendMessage(msg);

	m_cloud_file_view->hide();
	m_inviteFriend->hide();
	m_roster->hide();

	logout();
	m_docWindows.clear();
	close();
}

void STWhiteBoard::setAuthorityStatusSlot(QString subtype, QString jid, bool flag)
{
	// {"type":"authority","subtype":"show","flag":"true","courseID":"111","jid":"a"}
	// {"type":"authority","subtype":"operate","flag":"true","courseID":"111","jid":"a"}
	// {"type":"authority","subtype":"mic","flag":"true","courseID":"111","jid":"a"}
	QString value = flag ? "true" : "false";
	QString msg = QString("{\"type\":\"authority\",\"subtype\":\"%1\","
		"\"flag\":\"%2\",\"courseID\":\"%3\",\"jid\":\"%4\"}").arg(subtype, value, m_courseID, jid);
	m_messageClient->sendMessage(msg);
}

// ------------------------ STWhiteBoard ------------------------
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

	if (!m_all_stream_map.contains(id))
	{
		return;
	}
	if (m_all_stream_map[id].stream.get())
	{
		m_all_stream_map[id].subscription->Stop();
		m_all_stream_map.remove(id);
	}
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
			if (m_isAdmin)
			{
				sendLocalCamera(true);
				m_operate = true;
			}
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
	/*if (m_localCameraRenderID != -1)
	{
		Q_EMIT unshowLocalCameraSignal();
	}*/

	QList<QString> ids = m_all_stream_map.keys();

	QList<QString>::Iterator it;
	for (it = ids.begin(); it != ids.end(); it++)
	{
		unsubscribeStream(*it);
	}
	m_all_stream_map.clear();

	sendLocalCamera(false);
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
		if (m_local_camera_stream.get())
		{
			m_local_camera_stream->AttachVideoRenderer(m_videoItems[renderID]->getRenderWindow());
		}
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
		if (m_local_camera_stream.get())
		{
			m_local_camera_stream->AttachVideoRenderer(m_big_videoItems[renderID]->getRenderWindow());
		}
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

void STWhiteBoard::canOperate(bool flag, bool change)
{
	if (change)
	{
		m_canOperate = flag;
	}
	if (m_currentIndex == 0)
	{
		if (flag)
		{
			Q_EMIT toolbarModeSignal(ToolbarMode::WhiteBoard_Operator);
		}
		else
		{
			Q_EMIT toolbarModeSignal(ToolbarMode::WhiteBoard_Student);
		}
	}
}

void STWhiteBoard::sendLocalCamera(bool flag)
{
	if (flag)
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
		if (m_camera == "false")
		{
			m_camera = "true";
		}
		if (m_mic == "false")
		{
			m_mic = "true";
		}
		int width = STConfig::getConfig("/camera/width").toInt();
		int height = STConfig::getConfig("/camera/height").toInt();
		bool camera = (m_camera == "true");
		bool mic = (m_mic == "true");
		if (!m_local_camera_stream.get())
		{
			m_local_camera_stream_param.reset(new LocalCameraStreamParameters(camera, mic));
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
			m_show = true;
			m_camera = true;
			m_mic = true;
			TAHITI_INFO("发送本地视频成功...");
			Q_EMIT showLocalCameraSignal();
		},
			[=](unique_ptr<Exception> err)
		{
			TAHITI_ERROR("发送本地视频失败...");
		});

	}
	else
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
			m_show = false;
			if (m_camera == "true")
			{
				m_camera = "false";
			}
			if (m_mic == "true")
			{
				m_mic = "false";
			}
			TAHITI_INFO("停止发送本地视频成功...");
			Q_EMIT unshowLocalCameraSignal();
		}
	}
}

void STWhiteBoard::sendLocalAudio(bool flag)
{
	if (m_publication.get())
	{
		if (flag)
		{
			m_publication->Unmute(TrackKind::kAudio,
				[=]()
			{
				if (m_mic == "false")
				{
					m_mic = "true";
				}
				TAHITI_INFO("取消静音成功...");
			},
				[=](std::unique_ptr<Exception>)
			{
				TAHITI_INFO("取消静音失败...");
			});
		}
		else
		{
			m_publication->Mute(TrackKind::kAudio,
				[=]()
			{
				if (m_mic == "true")
				{
					m_mic = "false";
				}
				TAHITI_INFO("静音成功...");
			},
				[=](std::unique_ptr<Exception>)
			{
				TAHITI_INFO("静音失败...");
			});
		}
	}
}

// ------------------------ STWhiteBoard ------------------------
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
	STWBDocWindow* docWindow = new STWBDocWindow(path, m_docWindowIndex++, m_view);
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

void STWhiteBoard::openCloudFileView()
{
	int x = (geometry().width() - m_cloud_file_view->geometry().width()) / 2
		+ geometry().x();
	int y = (geometry().height() - m_cloud_file_view->geometry().height()) / 2
		+ geometry().y();
	m_cloud_file_view->move(QPoint(x, y));
	m_cloud_file_view->show();
	m_cloud_file_view->initCloudFileView();
}

void STWhiteBoard::closeCloudFileView()
{
	m_cloud_file_view->hide();
}

void STWhiteBoard::openInviteFriend()
{
	int x = (geometry().width() - m_inviteFriend->geometry().width()) / 2
		+ geometry().x();
	int y = (geometry().height() - m_inviteFriend->geometry().height()) / 2
		+ geometry().y();
	m_inviteFriend->move(QPoint(x, y));
	m_inviteFriend->show();
	m_inviteFriend->initFriendList();
}

void STWhiteBoard::closeInviteFriend()
{
	m_inviteFriend->hide();
}

void STWhiteBoard::openRoster()
{
	int x = (geometry().width() - m_roster->geometry().width()) / 2
		+ geometry().x();
	int y = (geometry().height() - m_roster->geometry().height()) / 2
		+ geometry().y();
	m_roster->move(QPoint(x, y));
	m_roster->show();
	m_roster->initRoster(m_courseID, m_group->getUserInfo(m_admin).userName, m_isAdmin);
}

void STWhiteBoard::closeRoster()
{
	m_roster->hide();
}

void STWhiteBoard::deleteCourse()
{
	STConfirm* m_confirm = new STConfirm(false, this);
	connect(m_confirm, SIGNAL(confirmOK()), this, SLOT(deleteCourseSlot()));
	m_confirm->setText(QStringLiteral("您是否确定结束本次课程？"));
	int parentX = geometry().x();
	int parentY = geometry().y();
	int parentWidth = geometry().width();
	int parentHeight = geometry().height();
	m_confirm->move(QPoint(parentX + (parentWidth - m_confirm->width()) / 2,
		parentY + (parentHeight - m_confirm->height()) / 2));
	m_confirm->exec();
}

void STWhiteBoard::setPenThickness(int thickness)
{
	m_pen_thickness = thickness;
	m_view->setPenThickness(thickness);
	hideStylePanels();
	/*if (m_vtoolbar->getCurrentSelect() != 2)
	{
		m_vtoolbar->on_pbPen_clicked();
	}*/
	Q_EMIT setPenThicknessSignal(thickness);
}

void STWhiteBoard::setPenColor(QString color)
{
	m_pen_color = color;
	m_view->setPenColor(color);
	m_vtoolbar->changePenShowColor(color);
	hideStylePanels();
	/*if (m_vtoolbar->getCurrentSelect() != 2)
	{
		m_vtoolbar->on_pbPen_clicked();
	}*/
	Q_EMIT setPenColorSignal(color);
}

void STWhiteBoard::setTextSize(int size)
{
	m_text_size = size;
	m_view->setTextSize(size);
	hideStylePanels();
	/*if (m_vtoolbar->getCurrentSelect() != 3)
	{
		m_vtoolbar->on_pbText_clicked();
	}*/
	Q_EMIT setTextSizeSignal(size);
}

void STWhiteBoard::setTextColor(QString color)
{
	m_text_color = color;
	m_view->setTextColor(color);
	m_vtoolbar->changeTextShowColor(color);
	hideStylePanels();
	/*if (m_vtoolbar->getCurrentSelect() != 3)
	{
		m_vtoolbar->on_pbText_clicked();
	}*/
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
	if ((watched == m_view || watched == ui.pgVideo) && e->type() == QEvent::Enter)
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
	int raiseHandPanelX = geometry().width() - m_raiseHandPanel->width() - 10;
	int raiseHandPanelY = geometry().height() - m_raiseHandPanel->height() - 30;
	m_raiseHandPanel->move(QPoint(raiseHandPanelX, raiseHandPanelY));

	int toolbarX = geometry().width() - m_vtoolbar->width() - 10;
	int toolbarY = geometry().height() / 2 - m_vtoolbar->height() / 2;
	m_vtoolbar->move(QPoint(toolbarX, toolbarY));
	m_penStylePanel->move(QPoint(toolbarX - m_penStylePanel->width(), toolbarY));
	m_textStylePanel->move(QPoint(toolbarX - m_textStylePanel->width(), toolbarY + 52));

	resizeMaximumDocWindow();
}

void STWhiteBoard::showEvent(QShowEvent* event)
{
	this->setAttribute(Qt::WA_Mapped);
	QWidget::showEvent(event);
}

void STWhiteBoard::drawRemoteRealtimePen(QString color, int thickness, QVector<QPoint> points)
{
	//setPenColor(color);
	//setPenThickness(thickness);
	m_view->drawRemoteRealtimePen(color, thickness, points);
}

void STWhiteBoard::drawRemotePenItem(QString color, int thickness, QVector<QPoint> points, QString itemID)
{
	//setPenColor(color);
	//setPenThickness(thickness);
	m_view->drawRemotePenItem(color, thickness, points, itemID);
}

void STWhiteBoard::drawRemoteTextItem(QString color, int size, QString content, QPoint pos, QString itemID)
{
	//setTextColor(color);
	//setTextSize(size);
	m_view->drawRemoteTextItem(color, size, content, pos, itemID);
}

void STWhiteBoard::moveRemoteItems(QPoint pos, QString itemID)
{
	m_view->moveRemoteItem(pos, itemID);
}

void STWhiteBoard::deleteRemoteItems(QList<QString> itemIDs)
{
	m_view->deleteRemoteItems(itemIDs);
}

void STWhiteBoard::createCourse(QString courseID, QString admin)
{
	// {"type":"course","action":"new","courseID":"111","admin":"st1@localhost"}
	QString msg = QString("{\"type\":\"course\",\"action\":\"new\",\"courseID\":\"%1\","
		"\"admin\":\"%2\"}").arg(courseID, admin);
	m_messageClient->sendMessage(msg);
}

QString STWhiteBoard::queryAdmin(QString courseID)
{
	// {"type":"course","action":"queryAdmin","courseID":"111"}
	QString msg = QString("{\"type\":\"course\",\"action\":\"queryAdmin\",\"courseID\":\"%1\"}").arg(courseID);
	return m_messageClient->sendMessage(msg);
}

void STWhiteBoard::joinCourse(QString courseID, QString jid, XmppGroup* group)
{
	m_jid = jid;
	m_group = group;
	m_courseID = courseID;
	m_admin = queryAdmin(m_courseID);
	m_name = m_group->getUserInfo(m_jid).userName;
	m_isAdmin = (m_admin == m_jid);

	m_messageClient->subscribeMessage(courseID);

	// {"type":"course","action":"join","courseID":"111","isAdmin":"false",
	// "jid":"a","name":"aaa","mic":"disable","camera":"disable"}
	QString isAdmin = m_isAdmin ? "true" : "false";
	m_camera = STConfig::getConfig("/camera/id").size() == 0 ? "disable" : "false";

	m_mic = "disable";
	QString micId = STConfig::getConfig("/mic/id");
	if (micId.size() > 0)
	{
		QList<QAudioDeviceInfo> audioInputs = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
		QList<QAudioDeviceInfo>::Iterator itAudio;
		for (itAudio = audioInputs.begin(); itAudio != audioInputs.end(); itAudio++)
		{
			if (itAudio->deviceName() == micId)
			{
				m_mic = "false";
				break;
			}
		}
	}

	m_camera = "disable";
	QString cameraId = STConfig::getConfig("/camera/id");
	if (cameraId.size() > 0)
	{
		QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
		QList<QCameraInfo>::Iterator itCamera;
		for (itCamera = cameras.begin(); itCamera != cameras.end(); itCamera++)
		{
			if (itCamera->deviceName() == cameraId)
			{
				m_mic = "false";
				break;
			}
		}
	}

	QString msg = QString("{\"type\":\"course\",\"action\":\"join\","
		"\"courseID\":\"%1\",\"isAdmin\":\"%2\",\"jid\":\"%3\","
		"\"name\":\"%4\",\"mic\":\"%5\",\"camera\":\"%6\"}").arg(courseID,
			isAdmin, m_jid, m_name, m_mic, m_camera);
	m_messageClient->sendMessage(msg);

	m_view->setCourseID(courseID);

	if (m_isAdmin)
	{
		m_raiseHandPanel->setVisible(false);
		Q_EMIT toolbarModeSignal(ToolbarMode::Video_Teacher);
	}
	else
	{
		Q_EMIT toolbarModeSignal(ToolbarMode::Video_Student);
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&m_tidKeepalive, &attr, keepAliveProc, this);
	}

	// 视频相关
	initConferenceClient();
	login();
}

void* STWhiteBoard::keepAliveProc(void* args)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL); //允许退出线程 
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL); //设置立即取消 
	STWhiteBoard* whiteboard = (STWhiteBoard*)args;
	while (1)
	{
		whiteboard->callSendKeepaliveSignal();
		QThread::sleep(15);
	}
	return NULL;
}

void STWhiteBoard::callSendKeepaliveSignal()
{
	Q_EMIT sendKeepaliveSignal();
}

void STWhiteBoard::sendKeepaliveSlot()
{
	// {"type":"course","action":"keepalive","courseID":"111","jid":"a"}
	QString msg = QString("{\"type\":\"course\",\"action\":\"keepalive\","
		"\"courseID\":\"%1\",\"jid\":\"%2\"}").arg(m_courseID, m_jid);
	m_messageClient->sendMessage(msg);
}

void STWhiteBoard::deleteCourseSlot()
{
	// {"type":"course","action":"del","courseID":"111"}
	QString msg = QString("{\"type\":\"course\",\"action\":\"del\",\"courseID\":\"%1\"}").arg(m_courseID);
	m_messageClient->sendMessage(msg);

	Q_EMIT deleteCourseSignal();
	on_pbClose_clicked();
}

void STWhiteBoard::whiteBoardMessageSlot(QString message)
{
	QJsonParseError complexJsonError;
	QJsonDocument complexParseDoucment = QJsonDocument::fromJson(message.toLatin1(), &complexJsonError);
	if (complexJsonError.error == QJsonParseError::NoError && complexParseDoucment.isObject())
	{
		QJsonObject jsonObject = complexParseDoucment.object();
		QJsonValue value;

		QString type;
		QString subtype;
		QJsonObject dataObject;
		QString itemID;
		QList<QString> itemIDs;
		// 先获得通用字段
		if (jsonObject.contains("type"))
		{
			value = jsonObject.take("type");
			if (value.isString())
			{
				type = value.toString();
			}
		}
		if (jsonObject.contains("subtype"))
		{
			value = jsonObject.take("subtype");
			if (value.isString())
			{
				subtype = value.toString();
			}
		}
		if (jsonObject.contains("data"))
		{
			value = jsonObject.take("data");
			if (value.isObject())
			{
				dataObject = value.toObject();
			}
		}
		// 根据分支处理
		if (type == "authority")
		{
			QString jid;
			bool flag;
			if (jsonObject.contains("flag"))
			{
				value = jsonObject.take("flag");
				if (value.isString())
				{
					flag = (value.toString() == "true");
				}
			}
			if (jsonObject.contains("jid"))
			{
				value = jsonObject.take("jid");
				if (value.isString())
				{
					jid = value.toString();
					if (jid == m_jid)
					{
						if (subtype == "show")
						{
							sendLocalCamera(flag);
							if (flag)
							{
								canOperate(m_canOperate);
							}
							else
							{
								canOperate(false, false);
							}
						}
						else if (subtype == "operate")
						{
							canOperate(flag);
						}
						else if (subtype == "mic")
						{
							sendLocalAudio(flag);
						}
					}
					Q_EMIT updateAuthorityStatusSignal(subtype, jid, flag);
				}
			}
		}
		else if (type == "wbrealtime")
		{
			QString action;
			if (dataObject.contains("action"))
			{
				value = dataObject.take("action");
				if (value.isString())
				{
					action = value.toString();
					if (action == "pen")
					{
						QString color;
						int thickness;
						QVector<QPoint> pointsList;
						if (dataObject.contains("color"))
						{
							value = dataObject.take("color");
							if (value.isString())
							{
								color = value.toString();
							}
						}
						if (dataObject.contains("thickness"))
						{
							value = dataObject.take("thickness");
							if (value.isDouble())
							{
								thickness = value.toDouble();
							}
						}
						if (dataObject.contains("points"))
						{
							value = dataObject.take("points");
							if (value.isArray())
							{
								QJsonArray points = value.toArray();
								for (int i = 0; i < points.size(); i += 2)
								{
									if (points.at(i).isDouble() && points.at(i + 1).isDouble())
									{
										pointsList.append(QPoint(points.at(i).toDouble(),
											points.at(i + 1).toDouble()));
									}
								}
							}
						}
						drawRemoteRealtimePen(color, thickness, pointsList);
					}
				}
			}
		}
		else if (type == "wbitem")
		{
			if (jsonObject.contains("id"))
			{
				value = jsonObject.take("id");
				if (value.isString())
				{
					itemID = value.toString();
				}
			}
			if (jsonObject.contains("ids"))
			{
				value = jsonObject.take("ids");
				if (value.isArray())
				{
					QJsonArray itemArray = value.toArray();
					for (int i = 0; i < itemArray.size(); i++)
					{
						if (itemArray.at(i).isString())
						{
							itemIDs.append(itemArray.at(i).toString());
						}
					}
				}
			}
			if (subtype == "add")
			{
				QString action;
				if (dataObject.contains("action"))
				{
					value = dataObject.take("action");
					if (value.isString())
					{
						action = value.toString();
					}
				}

				if (action == "pen")
				{
					QString color;
					int thickness;
					QVector<QPoint> pointsList;
					if (dataObject.contains("color"))
					{
						value = dataObject.take("color");
						if (value.isString())
						{
							color = value.toString();
						}
					}
					if (dataObject.contains("thickness"))
					{
						value = dataObject.take("thickness");
						if (value.isDouble())
						{
							thickness = value.toDouble();
						}
					}
					if (dataObject.contains("points"))
					{
						value = dataObject.take("points");
						if (value.isArray())
						{
							QJsonArray points = value.toArray();
							for (int i = 0; i < points.size(); i += 2)
							{
								if (points.at(i).isDouble() && points.at(i + 1).isDouble())
								{
									pointsList.append(QPoint(points.at(i).toDouble(),
										points.at(i + 1).toDouble()));
								}
							}
						}
					}
					drawRemotePenItem(color, thickness, pointsList, itemID);
				}
				else if (action == "text")
				{
					QString color;
					int size;
					QString content;
					int x;
					int y;
					if (dataObject.contains("color"))
					{
						value = dataObject.take("color");
						if (value.isString())
						{
							color = value.toString();
						}
					}
					if (dataObject.contains("size"))
					{
						value = dataObject.take("size");
						if (value.isDouble())
						{
							size = value.toDouble();
						}
					}
					if (dataObject.contains("content"))
					{
						value = dataObject.take("content");
						if (value.isString())
						{
							content = value.toString();
						}
					}
					if (dataObject.contains("pos"))
					{
						value = dataObject.take("pos");
						if (value.isArray())
						{
							QJsonArray posArray = value.toArray();
							if (posArray.at(0).isDouble())
							{
								x = posArray.at(0).toDouble();
							}
							if (posArray.at(1).isDouble())
							{
								y = posArray.at(1).toDouble();
							}
						}
					}
					drawRemoteTextItem(color, size, content, QPoint(x, y), itemID);
				}

			}
			else if (subtype == "del")
			{
				deleteRemoteItems(itemIDs);
			}
			else if (subtype == "move")
			{
				int x = 0;
				int y = 0;
				if (dataObject.contains("pos"))
				{
					value = dataObject.take("pos");
					if (value.isArray())
					{
						QJsonArray posArray = value.toArray();
						if (posArray.at(0).isDouble())
						{
							x = posArray.at(0).toDouble();
						}
						if (posArray.at(1).isDouble())
						{
							y = posArray.at(1).toDouble();
						}
					}
				}
				moveRemoteItems(QPoint(x, y), itemID);
			}
		}
		else if (type == "course")
		{
			if (jsonObject.contains("action"))
			{
				QString action;
				value = jsonObject.take("action");
				if (value.isString())
				{
					action = value.toString();
					if (action == "join" || action == "quit")
					{
						Q_EMIT refreshRosterSignal();
					}
				}
			}
		}
	}
	else
	{
		printf("error data\n%s", message.toUtf8().constData());
	}
}

// ------------------------ ConfSubObserver ------------------------
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
