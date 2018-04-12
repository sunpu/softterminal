#include "xmppclient.h"

using namespace std;
using namespace gloox;
using namespace tahiti;

CEventHandler::CEventHandler()
{
	m_count = 0;
}

CEventHandler::~CEventHandler()
{
}

void CEventHandler::resetHeartBeatCount()
{
	m_count = 0;
	return;
}

void CEventHandler::increaceHeartBeatCount()
{
	m_count++;
	return;
}

void CEventHandler::decreaceHeartBeatCount()
{
	if (m_count > 0)
	{
		m_count--;
	}
	return;
}

void CEventHandler::handleEvent(const Event& event)
{
	string sEvent;
	switch (event.eventType())
	{
	case Event::PingPing:
		sEvent = "PingPing";
		break;
	case Event::PingPong:
		sEvent = "PingPong";
		decreaceHeartBeatCount();
		break;
	case Event::PingError:
		sEvent = "PingError";
		break;
	default:
		break;
	}
	//printf("handleEvent:-------------%s\n", sEvent.c_str());
	return;
}

static string getIPAddress()
{
	QString localAddr = "";
	/*QList<QHostAddress> addrList = QNetworkInterface::allAddresses();
	for (int i = 0; i < addrList.count(); i++)
	{
		if (addrList[i].protocol() == QAbstractSocket::IPv4Protocol
			&& !(addrList[i].toString().contains("127.0.")))
		{
			localAddr = addrList[i].toString();
			break;
		}
	}*/

	return localAddr.toStdString();
}

static string getMacAddress()
{
	QString macAddr = "";
	/*QList<QNetworkInterface> macList = QNetworkInterface::allInterfaces();
	for (int i = 0; i < macList.count(); i++)
	{
		if (macList[i].flags().testFlag(QNetworkInterface::IsUp)
			&& macList[i].flags().testFlag(QNetworkInterface::IsRunning)
			&& !macList[i].flags().testFlag(QNetworkInterface::IsLoopBack))
		{
			macAddr = macList[i].hardwareAddress();
			break;
		}
	}
	macAddr = macAddr.replace(":", "");
	TAHITI_INFO("Mac address:[" << macAddr.toStdString().c_str() << "]");*/
	return macAddr.toStdString();
}

XmppClient::XmppClient()
{
	//m_ipAddress = getIPAddress();
	//m_macAddress = getMacAddress();
	m_ipAddress = "";
	m_macAddress = "";
	m_requestId = 0;
	m_xmppStatus = Presence::Unavailable;
	m_needLogin = true;
	m_client = NULL;

	qRegisterMetaType<QVariant>("QVariant");
}

XmppClient::~XmppClient()
{
	m_requestId = 0;
	m_xmppStatus = Presence::Unavailable;
	m_needLogin = true;
	if (m_client)
	{
		QMap<QString, MessageSession*>::Iterator it;
		for (it = m_sessionMap.begin(); it != m_sessionMap.end(); it++)
		{
			m_client->disposeMessageSession(it.value());
		}
	}
	m_client = NULL;
}

void XmppClient::setXmppAccount(QString user, QString passwd, QString server, QString port)
{
	m_xmppUser = user;
	m_xmppServerIP = server;
	m_xmppPasswd = passwd;
	m_xmppServerPort = port.toInt();
}

void XmppClient::createNewClient()
{
	char jidTmp[JID_LEN];
	memset(jidTmp, 0, JID_LEN);
	sprintf(jidTmp, "%s@%s/%s", m_xmppUser.toUtf8().constData(), m_xmppServerIP.toUtf8().constData(), DEV_TYPE);
	TAHITI_INFO("jid:" << jidTmp);

	JID jid(jidTmp);
	m_client = new Client(jid, m_xmppPasswd.toUtf8().constData(), m_xmppServerPort);

	m_client->registerConnectionListener(this);
	m_client->registerMessageSessionHandler(this, 0);
	m_client->rosterManager()->registerRosterListener(this, false);
	m_vManager = new VCardManager(m_client);
}

Presence::PresenceType XmppClient::getXmppStatus()
{
	return m_xmppStatus;
}

void XmppClient::setXmppStatus(Presence::PresenceType status)
{
	m_xmppStatus = status;
}

void XmppClient::notifyMyInfo()
{
	m_client->setPresence(m_xmppStatus, 0, "");
}

int XmppClient::getKeepaliveInterval()
{
	return m_keepaliveInterval;
}

int XmppClient::getKeepaliveCount()
{
	return m_keepaliveCount;
}

void* XmppClient::keepaliveProc(void* args)
{
	return NULL;
	CEventHandler* pEventHandler = new CEventHandler();
	XmppClient* xmppClient = (XmppClient*)args;
	Client* client = (Client*)xmppClient->getClient();
	while (true)
	{
		if (xmppClient->getXmppStatus() == Presence::Unavailable)
		{
			QThread::usleep(500);
			if (xmppClient->isNeedLogin())
			{
				TAHITI_INFO("keepalive try to login xmpp server...");
				xmppClient->login();
				pEventHandler->resetHeartBeatCount();
			}
			continue;
		}

		int interval = xmppClient->getKeepaliveInterval();
		int count = xmppClient->getKeepaliveCount();
		//QThread::sleep(interval);
		QThread::sleep(3);

		if (pEventHandler->getHeartBeatCount() > count)
		{
			TAHITI_ERROR("keepalive timeout, times > " << count << "!!!");
			xmppClient->setXmppStatus(Presence::Unavailable);
			continue;
		}
		client->xmppPing(JID(STR_EMPTY), pEventHandler);
		pEventHandler->increaceHeartBeatCount();
	}
	delete pEventHandler;
}

void XmppClient::run()
{
	createNewClient();
	login();
	pthread_create(&m_tidKeepalive, NULL, keepaliveProc, this);
}

/* 登录 */
void XmppClient::login()
{
	m_needLogin = true;
	if (m_xmppStatus == Presence::Chat || m_xmppStatus == Presence::DND)
	{
		return;
	}

	notifyMyInfo();

	m_selfInfo = {"", "", "", "", "", ""};

	pthread_create(&m_tidConnect, NULL, longConnectProc, m_client);
}

void* XmppClient::longConnectProc(void* args)
{
	TAHITI_INFO("longConnectProc...");
	Client* client = (Client*)args;
	client->connect();
	return NULL;
}

/* 登出 */
void XmppClient::logout()
{
	m_needLogin = false;
	if (m_xmppStatus == Presence::Unavailable)
	{
		return;
	}
	m_friendList.clear();
	m_requestId = 0;
	m_xmppStatus = Presence::Unavailable;
	m_needLogin = true;
	if (m_client)
	{
		QMap<QString, MessageSession*>::Iterator it;
		for (it = m_sessionMap.begin(); it != m_sessionMap.end(); it++)
		{
			m_client->disposeMessageSession(it.value());
		}
		m_client->disconnect();
	}
	m_responseMap.clear();
	setXmppStatus(Presence::Unavailable);
	m_client = NULL;
}

/* 连接上 */
void XmppClient::onConnect()
{
	TAHITI_INFO("connect server success!");

	setXmppStatus(Presence::Chat);
	notifyMyInfo();
	m_selfInfo.jid = m_client->jid().bare().c_str();
	queryVCard(m_selfInfo.jid);

	Q_EMIT loginResult(true);

	m_client->disco()->getDiscoItems(JID("conference.localhost"), "", this, GetRoomItems, "");
	//JID nick("test@conference.jabber.org/glooxmuctest");
	//MUCRoom* room = new MUCRoom(m_client, nick, this, 0);
}

void XmppClient::handleDiscoItems(const JID& from, const Disco::Items& items, int context)
{
	switch (context)
	{
	case GetRoomItems:
	{
		JID nick("222@conference.localhost");
		MUCRoom* room = new MUCRoom(m_client, nick, this, 0);
		room->getRoomItems();
		room->getRoomInfo();
		break;
	}
	default:
		break;
	}
}
void XmppClient::handleDiscoInfo(const JID& /*from*/, const Disco::Info& info, int context)
{
	switch (context)
	{
	case GetRoomInfo:
	{
		break;
	}
	default:
		break;
	}
}

void XmppClient::handleDiscoError(const JID& /*from*/, const Error* /*error*/, int context)
{
	switch (context)
	{
	case GetRoomInfo:
		break;
	case GetRoomItems:
		break;
	default:
		break;
	}
}

/* 连接丢失 */
void XmppClient::onDisconnect(ConnectionError e)
{
	TAHITI_INFO("disconnect server!");
	setXmppStatus(Presence::Unavailable);
	//delete(m_client);
	Q_EMIT loginResult(false);
}

bool XmppClient::onTLSConnect(const CertInfo& info)
{
	return true;
}

/* 获得花名册 */
void XmppClient::queryRoster()
{
	m_friendList.clear();
	m_client->rosterManager()->synchronize();
	Roster* roster = m_client->rosterManager()->roster();
	Roster::const_iterator it;
	for (it = roster->begin(); it != roster->end(); it++)
	{
		UserInfo info;
		info.jid = (*it).second->jidJID().full().c_str();
		StringList g = (*it).second->groups();
		m_friendList.append(info);

		// 获取card信息
		queryVCard((*it).second->jidJID().full().c_str());
	}
}

QList<UserInfo> XmppClient::getRoster()
{
	return m_friendList;
}

UserInfo XmppClient::getSelfInfo()
{
	return m_selfInfo;
}

/* 修改密码 */
void XmppClient::modifyPasswd(QString password)
{
	m_xmppTempPasswd = password;
	Registration* reg = new Registration(m_client);
	reg->registerRegistrationHandler(this);
	reg->changePassword(m_xmppUser.toUtf8().constData(), password.toUtf8().constData());
}

void XmppClient::handleRegistrationFields(const JID& /*from*/, int fields, std::string instructions)
{
	printf("fields: %d\ninstructions: %s\n", fields, instructions.c_str());
}

void XmppClient::handleRegistrationResult(const JID& /*from*/, RegistrationResult result)
{
	printf("result: %d\n", result);
	if (result == RegistrationSuccess)
	{
		m_xmppPasswd = m_xmppTempPasswd;
		QString rememberPasswd = STConfig::getConfig("/config/rememberPasswd");
		if (rememberPasswd == "true")
		{
			STConfig::setConfig("/xmpp/passwd", m_xmppPasswd);
		}
	}
}

void XmppClient::handleAlreadyRegistered(const JID& /*from*/)
{
	printf("the account already exists.\n");
}

void XmppClient::handleDataForm(const JID& /*from*/, const DataForm& /*form*/)
{
	printf("datForm received\n");
}

void XmppClient::handleOOB(const JID& /*from*/, const OOB& oob)
{
	printf("OOB registration requested. %s: %s\n", oob.desc().c_str(), oob.url().c_str());
}

/* 查询个人资料 */
void XmppClient::queryVCard(QString id)
{
	JID jid(id.toUtf8().constData());
	m_vManager->fetchVCard(jid, this);
}

void XmppClient::handleVCard(const JID& jid, const VCard *v)
{
	if (!v)
	{
		printf("empty vcard!\n");
		return;
	}

	m_current_vcard = new VCard(*v);

	// 删除旧图片
	QString desPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
		+ DATA_ROOT_PATH + AVATAR_PATH;
	QString oldFileName;
	QDir dir;
	dir.setPath(desPath);
	QDirIterator iter(dir, QDirIterator::Subdirectories);
	while (iter.hasNext())
	{
		iter.next();
		QFileInfo info = iter.fileInfo();
		if (info.isFile() && info.fileName().startsWith(jid.full().c_str()))
		{
			oldFileName = QString(desPath) + info.fileName();
			QFile::remove(oldFileName);
		}
	}

	// 更新新图片
	QString path = desPath + QString(jid.full().c_str());
	QString photoType;
	photoType = m_current_vcard->photo().type.c_str();
	if (photoType == "image/jpeg")
	{
		path = path + ".jpg";
	}
	else if (photoType == "image/png")
	{
		path = path + ".png";
	}
	string photoContent;
	photoContent = m_current_vcard->photo().binval;
	if (photoContent.size() != 0)
	{
		QFile file(path);
		file.open(QIODevice::WriteOnly);
		file.write(photoContent.c_str(), photoContent.size());
		file.close();
	}

	// 如果是自己，更新本人资料
	if (m_selfInfo.jid == jid.full().c_str())
	{
		m_self_vcard = new VCard(*v);
		if (photoContent.size() != 0)
		{
			m_selfInfo.photoPath = path;
		}
		if (m_self_vcard->nickname().size() == 0)
		{
			m_selfInfo.userName = m_selfInfo.jid;
		}
		else
		{
			m_selfInfo.userName = m_self_vcard->nickname().c_str();
		}
		if (m_self_vcard->telephone().size() > 0)
		{
			m_selfInfo.telephone = m_self_vcard->telephone().front().number.c_str();
		}
		if (m_self_vcard->emailAddresses().size() > 0)
		{
			m_selfInfo.email = m_self_vcard->emailAddresses().front().userid.c_str();
		}
		m_selfInfo.description = m_self_vcard->desc().c_str();
		return;
	}

	// 如果已经是好友，将更新当前资料
	QList<UserInfo>::iterator it;
	for (it = m_friendList.begin(); it != m_friendList.end(); it++)
	{
		if (it->jid == jid.full().c_str())
		{
			if (photoContent.size() != 0)
			{
				it->photoPath = path;
			}
			if (m_current_vcard->nickname().size() == 0)
			{
				it->userName = it->jid;
			}
			else
			{
				it->userName = m_current_vcard->nickname().c_str();
			}
			if (m_current_vcard->telephone().size() > 0)
			{
				it->telephone = m_current_vcard->telephone().front().number.c_str();
			}
			if (m_current_vcard->emailAddresses().size() > 0)
			{
				it->email = m_current_vcard->emailAddresses().front().userid.c_str();
			}
			it->description = m_current_vcard->desc().c_str();
			return;
		}
	}

	// 如果不是好友，将返回用户资料
	UserInfo userInfo;
	userInfo.jid = jid.full().c_str();
	userInfo.userName = m_current_vcard->nickname().c_str();
	if (photoContent.size() != 0)
	{
		userInfo.photoPath = path;
	}
	if (m_current_vcard->telephone().size() > 0)
	{
		userInfo.telephone = m_current_vcard->telephone().front().number.c_str();
	}
	if (m_current_vcard->emailAddresses().size() > 0)
	{
		userInfo.email = m_current_vcard->emailAddresses().front().userid.c_str();
	}
	userInfo.description = m_current_vcard->desc().c_str();
	QVariant userInfoVar;
	userInfoVar.setValue(userInfo);
	Q_EMIT contactFoundResult(1, userInfoVar);
}

/* 修改个人资料 */
void XmppClient::modifyVCard(UserInfo userInfo)
{
	VCard* vcard = new VCard(*m_self_vcard);
	vcard->setNickname(userInfo.userName.toUtf8().constData());
	vcard->addTelephone(userInfo.telephone.toUtf8().constData(), VCard::AddrTypeWork);
	vcard->addEmail(userInfo.email.toUtf8().constData(), VCard::AddrTypeWork);
	vcard->setDesc(userInfo.description.toUtf8().constData());

	m_vManager->storeVCard(vcard, this);
	m_selfInfo = userInfo;
}

/* 修改个人头像 */
void XmppClient::modifySelfPic(QString picFile)
{
	VCard* vcard = new VCard(*m_self_vcard);

	/* 图片 */
	QFileInfo fi = QFileInfo(picFile);
	QString fileType;
	QString suffix = fi.suffix().toLower();
	if (suffix == "jpg" || suffix == "jpeg")
	{
		fileType = "image/jpeg";
	}
	else if (suffix == "png")
	{
		fileType = "image/png";
	}

	if (QFile::exists(picFile))
	{
		string photoData;
		string type = fileType.toStdString();

		QFile file(picFile);
		file.open(QIODevice::ReadOnly);
		QByteArray dataByteArray = file.readAll();
		photoData = dataByteArray.toStdString();

		vcard->setPhoto(type, photoData);

		m_vManager->storeVCard(vcard, this);

		m_selfInfo.photoPath = picFile;
	}
}

void XmppClient::handleVCardResult(VCardContext context, const JID& jid,
	StanzaError se)
{
	printf("vcard result: context: %d, jid: %s, error: %d\n", context, jid.full().c_str(), se);
}

/* 消息发送 */
void XmppClient::sendMsg(QString dest, QString msg)
{
	JID jid(dest.toUtf8().constData());
	MessageSession* mess = new MessageSession(m_client, jid);
	mess->registerMessageHandler(this);
	m_msgEventFilter = new MessageEventFilter(mess);
	m_msgEventFilter->registerMessageEventHandler(this);
	mess->send(msg.toUtf8().constData());
}

/* 消息接收 */
void XmppClient::handleMessageEvent(const JID& from, MessageEventType event)
{
	printf("received event: %d from: %s\n", event, from.full().c_str());
}

void XmppClient::handleMessageSession(MessageSession* session)
{
	printf("got new session\n");
	// this example can handle only one session. so we get rid of the old session
	if (!m_sessionMap.contains(session->target().bare().c_str()))
	{
		MessageSession* mess = session;
		//m_client->disposeMessageSession(mess);
		mess->registerMessageHandler(this);
		m_msgEventFilter = new MessageEventFilter(mess);
		m_msgEventFilter->registerMessageEventHandler(this);
		m_sessionMap[session->target().bare().c_str()] = mess;
	}
}

void XmppClient::handleMessage(const Message& msg, MessageSession* session)
{
	TAHITI_INFO("type:" << msg.subtype());

	if (msg.subtype() == Message::Chat && msg.body().size() != 0)
	{
		TAHITI_INFO("type:" << msg.subtype() << ", message:" << msg.body().c_str());
		Q_EMIT showMessage(msg.from().username().c_str(), msg.body().c_str());
	}
}

void XmppClient::handleLog(gloox::LogLevel level, LogArea area, const string& message)
{
	char buff[MSG_LEN] = { '\0' };
	sprintf(buff, "log: level: %d, area: %d, %s\n", level, area, message.c_str());
	TAHITI_INFO(buff);
}

void XmppClient::handleItemSubscribed(const JID& jid)
{
	printf("subscribed %s\n", jid.bare().c_str());
	//callRefreshSignalProc();
}

void XmppClient::callRefreshSignalProc()
{
	pthread_create(&m_tidRefreshSignal, NULL, refreshSignalProc, this);
}

void XmppClient::refreshSignal()
{
	Q_EMIT refreshContactSignal();
}

void* XmppClient::refreshSignalProc(void* args)
{
	QThread::sleep(2);
	XmppClient* xmppClient = (XmppClient*)args;
	xmppClient->queryRoster();
	QThread::sleep(2);
	xmppClient->refreshSignal();
	return NULL;
}

void XmppClient::handleItemAdded(const JID& jid)
{
	printf("added %s\n", jid.bare().c_str());
	callRefreshSignalProc();
}

void XmppClient::handleItemUnsubscribed(const JID& jid)
{
	printf("unsubscribed %s\n", jid.bare().c_str());
	//callRefreshSignalProc();
}

void XmppClient::handleItemRemoved(const JID& jid)
{
	printf("removed %s\n", jid.bare().c_str());
	callRefreshSignalProc();
}

void XmppClient::handleItemUpdated(const JID& jid)
{
	printf("updated %s\n", jid.bare().c_str());
}

void XmppClient::handleRoster(const Roster& roster)
{
	printf("roster arriving\nitems:\n");
}

void XmppClient::handleRosterError(const IQ& /*iq*/)
{
	printf("a roster-related error occured\n");
}

/* 好友出席状态信息 */
void XmppClient::handleRosterPresence(const RosterItem& item, const string& resource,
	Presence::PresenceType presence, const string& /*msg*/)
{
	printf("presence received: %s/%s -- %d\n", item.jidJID().full().c_str(), resource.c_str(), presence);
}

void XmppClient::handleSelfPresence(const RosterItem& item, const string& resource,
	Presence::PresenceType presence, const string& /*msg*/)
{
	printf("self presence received: %s/%s -- %d\n", item.jidJID().full().c_str(), resource.c_str(), presence);
}

/* 订阅对方 */
bool XmppClient::subscribeOther(QString jid)
{
	StringList groups;
	JID id(jid.toUtf8().constData());
	m_client->rosterManager()->subscribe(id, STR_EMPTY, groups, STR_EMPTY);
	return true;
}

/* 取关对方 */
bool XmppClient::unsubscribeOther(QString jid)
{
	JID id(jid.toUtf8().constData());
	m_client->rosterManager()->unsubscribe(id, STR_EMPTY);
	m_client->rosterManager()->remove(id);
	return true;
}

/* 处理对方的订阅请求 */
bool XmppClient::handleSubscriptionRequest(const JID& jid, const string& /*msg*/)
{
	printf("subscription: %s\n", jid.bare().c_str());
	Roster* roster = m_client->rosterManager()->roster();
	Roster::const_iterator it;
	for (it = roster->begin(); it != roster->end(); it++)
	{
		if ((*it).second->jidJID().full().compare(jid.full()) == 0)
		{
			m_client->rosterManager()->ackSubscriptionRequest(jid, true);
			return true;
		}
	}
	m_subscriptionRequestList.append(jid.bare().c_str());
	Q_EMIT subscriptionRequest(jid.bare().c_str());
	return true;
}

void XmppClient::ackSubscriptionRequest(QString jid, bool ack)
{
	JID id(jid.toUtf8().constData());
	m_client->rosterManager()->ackSubscriptionRequest(id, ack);
	StringList groups;
	m_client->rosterManager()->subscribe(id, STR_EMPTY, groups, STR_EMPTY);
	m_subscriptionRequestList.removeOne(jid);
}

/* 处理对方的取关请求 */
bool XmppClient::handleUnsubscriptionRequest(const JID& jid, const string& /*msg*/)
{
	printf("unsubscription: %s\n", jid.bare().c_str());
	m_client->rosterManager()->unsubscribe(jid, STR_EMPTY);
	m_client->rosterManager()->remove(jid);
	m_client->rosterManager()->cancel(jid, STR_EMPTY);
	return true;
}

void XmppClient::handleNonrosterPresence(const Presence& presence)
{
	printf("received presence from entity not in the roster: %s\n", presence.from().full().c_str());
}


void XmppClient::handleMUCParticipantPresence(MUCRoom * /*room*/, const MUCRoomParticipant participant,
	const Presence& presence)
{
	if (presence.presence() == Presence::Available)
		printf("!!!!!!!!!!!!!!!! %s is in the room, too\n", participant.nick->resource().c_str());
	else if (presence.presence() == Presence::Unavailable)
		printf("!!!!!!!!!!!!!!!! %s left the room\n", participant.nick->resource().c_str());
	else
		printf("Presence is %d of %s\n", presence.presence(), participant.nick->resource().c_str());
}

void XmppClient::handleMUCMessage(MUCRoom* /*room*/, const Message& msg, bool priv)
{
	printf("%s said: '%s' (history: %s, private: %s)\n", msg.from().resource().c_str(), msg.body().c_str(),
		msg.when() ? "yes" : "no", priv ? "yes" : "no");
}

void XmppClient::handleMUCSubject(MUCRoom * /*room*/, const std::string& nick, const std::string& subject)
{
	if (nick.empty())
		printf("Subject: %s\n", subject.c_str());
	else
		printf("%s has set the subject to: '%s'\n", nick.c_str(), subject.c_str());
}

void XmppClient::handleMUCError(MUCRoom * /*room*/, StanzaError error)
{
	printf("!!!!!!!!got an error: %d", error);
}

void XmppClient::handleMUCInfo(MUCRoom * /*room*/, int features, const std::string& name,
	const DataForm* infoForm)
{
	//printf("features: %d, name: %s, form xml: %s\n",
	//	features, name.c_str(), infoForm->tag()->xml().c_str());
}

void XmppClient::handleMUCItems(MUCRoom * /*room*/, const Disco::ItemList& items)
{
	Disco::ItemList::const_iterator it = items.begin();
	for (; it != items.end(); ++it)
	{
		printf("%s -- %s is an item here\n", (*it)->jid().full().c_str(), (*it)->name().c_str());
	}
}

void XmppClient::handleMUCInviteDecline(MUCRoom * /*room*/, const JID& invitee, const std::string& reason)
{
	printf("Invitee %s declined invitation. reason given: %s\n", invitee.full().c_str(), reason.c_str());
}

bool XmppClient::handleMUCRoomCreation(MUCRoom *room)
{
	printf("room %s didn't exist, beeing created.\n", room->name().c_str());
	return true;
}

Client* XmppClient::getClient()
{
	return m_client;
}

















/*string XmppClient::deviceName(string deviceName)
{
	if (!deviceName.empty())
	{
		TAHITI_INFO("set deviceName:[" << deviceName.c_str() << "]");
		m_deviceName = deviceName;
	}
	TAHITI_INFO("get deviceName:[" << m_deviceName.c_str() << "]");
	return m_deviceName;
}

string XmppClient::xmppAccount(string xmppAccount)
{
	if (!xmppAccount.empty())
	{
		TAHITI_INFO("set xmppAccount:[" << xmppAccount.c_str() << "]");
		m_xmppAccount = xmppAccount;

		Json::Value xmppAccountJson;
		Json::Reader reader;
		if (reader.parse(xmppAccount, xmppAccountJson))
		{
			m_xmppUser = xmppAccountJson["user"].asCString();
			m_xmppPasswd = xmppAccountJson["passwd"].asCString();
			m_xmppServerIP = xmppAccountJson["server"].asCString();
			m_xmppID = xmppAccountJson["id"].asCString();
			TAHITI_INFO("user:[" << m_xmppUser.c_str() << "],passwd:[" << m_xmppPasswd.c_str()
				<< "],server:[" << m_xmppServerIP.c_str() << "],id:[" << m_xmppID.c_str()
				<< "]");
			createNewClient();
		}
	}
	TAHITI_INFO("get xmppAccount:[" << m_xmppAccount.c_str() << "]");
	return m_xmppAccount;
}

string XmppClient::xmppStatus()
{
	string status;
	switch (m_xmppStatus)
	{
	case Presence::Chat:
		status = "chat";
		break;
	case Presence::DND:
		status = "dnd";
		break;
	case Presence::Unavailable:
		status = "offline";
		break;
	default:
		status = "unknown";
		break;
	}
	TAHITI_INFO("get xmppStatus:[" << m_xmppStatus << "]");
	return status;
}

string XmppClient::xmppLogin()
{
	TAHITI_INFO("xmppLogin...");
	login();
	return xmppStatus();
}

string XmppClient::xmppLogout()
{
	TAHITI_INFO("xmppLogout...");
	logout();
	return xmppStatus();
}

string XmppClient::xmppKeepalive(string xmppKeepalive)
{
	if (!xmppKeepalive.empty())
	{
		TAHITI_INFO("set xmppKeepalive:[" << xmppKeepalive.c_str() << "]");
		m_xmppKeepalive = xmppKeepalive;

		Json::Value xmppKeepaliveJson;
		Json::Reader reader;
		if (reader.parse(xmppKeepalive, xmppKeepaliveJson))
		{
			m_keepaliveInterval = xmppKeepaliveJson["interval"].asInt();
			m_keepaliveCount = xmppKeepaliveJson["count"].asInt();
			TAHITI_INFO("interval:[" << m_keepaliveInterval << "],count:["
				<< m_keepaliveCount << "]");
		}
	}
	TAHITI_INFO("get xmppKeepalive:[" << m_xmppKeepalive.c_str() << "]");
	return m_xmppKeepalive;
}
*/
