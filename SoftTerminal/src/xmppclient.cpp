#include "xmppclient.h"

using namespace std;
using namespace gloox;
using namespace tahiti;

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
	m_isWorking = false;

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
	m_client->setPresence(m_xmppStatus, 0);
}

int XmppClient::getKeepaliveInterval()
{
	return m_keepaliveInterval;
}

int XmppClient::getKeepaliveCount()
{
	return m_keepaliveCount;
}

void XmppClient::run()
{
	createNewClient();
	login();
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
	m_mucGroupList.clear();
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
	m_isWorking = false;
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

	m_client->disco()->getDiscoItems(JID("conference.localhost"), "", this, GetRoomItems);

	pthread_create(&m_tidLoadOfflineMsg, NULL, loadOfflineMsgProc, this);
}

void* XmppClient::loadOfflineMsgProc(void* args)
{
	TAHITI_INFO("loadOfflineMsgProc...");
	QThread::sleep(5);
	XmppClient* xmppClient = (XmppClient*)args;
	xmppClient->processOfflineMsg();
	return NULL;
}

void XmppClient::processOfflineMsg()
{
	m_isWorking = true;
	QList<PersonMsg>::Iterator it;
	for (it = m_offlineMsgList.begin(); it != m_offlineMsgList.end(); it++)
	{
		Q_EMIT showMessage(it->jid, it->msg, it->time);
	}
}

void XmppClient::handleDiscoItems(const JID& from, const Disco::Items& items, int context)
{
	switch (context)
	{
	case GetRoomItems:
	{
		QList<QString> idList;
		Disco::ItemList list = items.items();
		Disco::ItemList::iterator it;
		for (it = list.begin(); it != list.end(); it++)
		{
			idList.append((*it)->jid().bare().c_str());
		}
		qSort(idList.begin(), idList.end());
		QList<QString>::iterator iter;
		for (iter = idList.begin(); iter != idList.end(); iter++)
		{
			QString nick = QString("%1/%2").arg(*iter, m_xmppUser);
			XmppGroup* group = new XmppGroup(this, nick);
			connect(group, SIGNAL(joinGroupResultSignal(bool)), this, SLOT(joinGroupResultSlot(bool)));
			m_mucGroupList.append(group);
		}
		break;
	}
	default:
		break;
	}
}

void XmppClient::joinGroupResultSlot(bool result)
{
	if (!result)
	{
		m_mucGroupList.removeOne((XmppGroup*)sender());
	}
}

void XmppClient::createGroupResultSlot(QString id)
{
	Q_EMIT createGroupResultSignal(id);
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
	/*string photoContent;
	photoContent = m_current_vcard->photo().binval;
	if (photoContent.size() != 0)
	{
		QFile file(path);
		file.open(QIODevice::WriteOnly);
		file.write(photoContent.c_str(), photoContent.size());
		file.close();
	}*/

	// 返回用户资料
	UserInfo userInfo;
	userInfo.jid = jid.full().c_str();
	userInfo.userName = m_current_vcard->nickname().c_str();
	userInfo.photoPath = path;
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

	// 如果是自己，更新本人资料
	if (m_selfInfo.jid == jid.full().c_str())
	{
		m_self_vcard = new VCard(*v);
		m_selfInfo.photoPath = path;
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
			it->photoPath = path;
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
		string type = fileType.toUtf8().constData();

		QFile file(picFile);
		file.open(QIODevice::ReadOnly);
		QByteArray dataByteArray = file.readAll();
		photoData = dataByteArray.toStdString();

		vcard->setPhoto(type, photoData);

		m_vManager->storeVCard(vcard, this);

		m_selfInfo.photoPath = picFile;
		file.close();
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
		printf("%s said: '%s' (history: %s)\n", msg.from().resource().c_str(), msg.body().c_str(),
			msg.when() ? "yes" : "no");
		TAHITI_INFO("type:" << msg.subtype() << ", message:" << msg.body().c_str());
		QString jid = msg.from().username().c_str();
		QString message = msg.body().c_str();
		QString time;
		if (msg.when())
		{
			time = QDateTime::fromString(msg.when()->stamp().c_str(), Qt::ISODate).toString("yyyy-MM-dd hh:mm:ss");
		}
		else
		{
			time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
		}
		if (!m_isWorking)
		{
			PersonMsg personMsg;
			personMsg.jid = jid;
			personMsg.time = time;
			personMsg.msg = message;
			m_offlineMsgList.append(personMsg);
			return;
		}
		Q_EMIT showMessage(jid, message, time);
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

Client* XmppClient::getClient()
{
	return m_client;
}

void XmppClient::createGroup(GroupInfo info, QList<QString> members)
{
	QDateTime time = QDateTime::currentDateTime();
	QString timeStr = time.toString("yyyyMMddhhmmsszzz");
	QList<XmppGroup*>::const_iterator it;
	for (it = m_mucGroupList.constBegin(); it != m_mucGroupList.constEnd(); it++)
	{
		if ((*it)->getGroupInfo().id == timeStr)
		{
			Q_EMIT createGroupResultSignal("");
			return;
		}
	}
	QString nick = QString("%1@conference.localhost/%2").arg(timeStr, m_xmppUser);
	XmppGroup* group = new XmppGroup(this, nick);
	connect(group, SIGNAL(createGroupResultSignal(QString)), this, SLOT(createGroupResultSlot(QString)));
	group->setGroupInfo(info);
	group->setMembers(members);
	m_mucGroupList.append(group);
}

void XmppClient::removeGroup(QString id)
{
	QList<XmppGroup*>::const_iterator it;
	for (it = m_mucGroupList.constBegin(); it != m_mucGroupList.constEnd(); it++)
	{
		if ((*it)->getGroupInfo().id == id)
		{
			(*it)->remove();
			m_mucGroupList.removeOne((*it));
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
XmppGroup::XmppGroup(XmppClient* client, QString nick) : m_client(client)
{
	connect(m_client, SIGNAL(contactFoundResult(int, QVariant)), this, SLOT(onContactFoundResult(int, QVariant)));

	m_info.id = nick.split("@")[0];
	m_owner = nick.split("/")[1].append("@localhost");
	m_client->queryVCard(m_owner);

	m_room = new MUCRoom(client->getClient(), JID(nick.toUtf8().constData()), this, this);
	m_room->join();
	m_room->requestList(RequestOwnerList);
	m_room->requestList(RequestMemberList);
	m_room->getRoomInfo();
	//m_room->getRoomItems();
	//m_room->requestRoomConfig();
	m_isWorking = false;

	pthread_create(&m_tidLoadOfflineMsg, NULL, loadOfflineMsgProc, this);
}

XmppGroup::~XmppGroup()
{
	m_room->leave();
	delete m_room;
	m_isWorking = false;
}

void XmppGroup::remove()
{
	m_room->destroy();
	m_isWorking = false;
}


void* XmppGroup::loadOfflineMsgProc(void* args)
{
	TAHITI_INFO("loadOfflineMsgProc...");
	QThread::sleep(5);
	XmppGroup* group = (XmppGroup*)args;
	group->processOfflineMsg();
	return NULL;
}

void XmppGroup::processOfflineMsg()
{
	m_isWorking = true;
	QList<GroupMsg>::Iterator it;
	for (it = m_offlineMsgList.begin(); it != m_offlineMsgList.end(); it++)
	{
		Q_EMIT showGroupMessage(it->jid, it->user, it->msg, it->time);
	}
}

void XmppGroup::setMembers(QList<QString> members)
{
	MUCListItemList banList;
	QList<QString>::iterator iter;
	for (iter = m_members.begin(); iter != m_members.end(); iter++)
	{
		if (!members.contains(*iter))
		{
			MUCListItem item(JID(iter->toUtf8().constData()), RoleNone, AffiliationNone, "");
			banList.push_back(item);
		}
	}
	m_room->storeList(banList, StoreBanList);

	m_members = members;
	QList<QString>::iterator it;
	MUCListItemList items;
	for (it = m_members.begin(); it != m_members.end(); it++)
	{
		MUCListItem item(JID(it->toUtf8().constData()), RoleParticipant, AffiliationMember, "");
		items.push_back(item);
		m_client->queryVCard(*it);
	}
	m_room->storeList(items, StoreMemberList);
	m_room->requestList(RequestMemberList);
}

void XmppGroup::setGroupInfo(GroupInfo info)
{
	m_info.name = info.name;
	m_info.description = info.description;

	DataForm* form = new DataForm(TypeSubmit);
	form->addField(DataFormField::FieldType::TypeBoolean,
		"muc#roomconfig_persistentroom", "true");
	form->addField(DataFormField::FieldType::TypeBoolean,
		"muc#roomconfig_membersonly", "true");
	form->addField(DataFormField::FieldType::TypeTextSingle,
		"muc#roomconfig_roomname", m_info.name.toUtf8().constData());
	form->addField(DataFormField::FieldType::TypeTextSingle,
		"muc#roomconfig_roomdesc", m_info.description.toUtf8().constData());
	m_room->setRoomConfig(form);
	m_room->getRoomInfo();
	//m_room->requestRoomConfig();
	//m_room->setRequestHistory(50, MUCRoom::HistoryMaxStanzas);
}

void XmppGroup::sendMsg(QString msg)
{
	m_room->send(msg.toUtf8().constData());
}

void XmppGroup::handleMUCParticipantPresence(MUCRoom * /*room*/, const MUCRoomParticipant participant,
	const Presence& presence)
{
	printf("!!!!!!!!!!!!!!!! %s, %d\n", participant.nick->resource().c_str(), presence.presence());
	/*if (presence.presence() == Presence::Available)
		printf("!!!!!!!!!!!!!!!! %s is in the room, too\n", participant.nick->resource().c_str());
	else if (presence.presence() == Presence::Unavailable)
		printf("!!!!!!!!!!!!!!!! %s left the room\n", participant.nick->resource().c_str());
	else
		printf("Presence is %d of %s\n", presence.presence(), participant.nick->resource().c_str());*/
	if (presence.presence() == Presence::Available || presence.presence() == Presence::Chat
		|| presence.presence() == Presence::Away || presence.presence() == Presence::DND)
	{
		m_onlines.append(participant.nick->resource().c_str());
	}
	else
	{
		m_onlines.removeOne(participant.nick->resource().c_str());
	}
	Q_EMIT refreshOnlineSignal();
}

void XmppGroup::handleMUCMessage(MUCRoom* /*room*/, const Message& msg, bool priv)
{
	if (m_client->getSelfInfo().jid == QString(msg.from().resource().c_str()).append("@localhost"))
	{
		return;
	}

	if (msg.subtype() == Message::Groupchat && msg.body().size() != 0)
	{
		QString time;
		if (msg.when())
		{
			time = QDateTime::fromString(msg.when()->stamp().c_str(), Qt::ISODate).toString("yyyy-MM-dd hh:mm:ss");
		}
		else
		{
			time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
		}
		printf("%s said: '%s' (time: %s)\n", msg.from().resource().c_str(),
			msg.body().c_str(), time.toUtf8().constData()
);
		
		if (!m_isWorking)
		{
			GroupMsg groupMsg;
			groupMsg.jid = msg.from().username().c_str();
			groupMsg.user = msg.from().resource().c_str();
			groupMsg.msg = msg.body().c_str();
			groupMsg.time = time;
			m_offlineMsgList.append(groupMsg);
			return;
		}
		TAHITI_INFO("user:" << msg.from().resource().c_str() << ", message:" << msg.body().c_str());
		Q_EMIT showGroupMessage(msg.from().username().c_str(), msg.from().resource().c_str(), msg.body().c_str(), time);
	}
}

void XmppGroup::handleMUCSubject(MUCRoom * /*room*/, const std::string& nick, const std::string& subject)
{
	if (nick.empty())
		printf("Subject: %s\n", subject.c_str());
	else
		printf("%s has set the subject to: '%s'\n", nick.c_str(), subject.c_str());
}

void XmppGroup::handleMUCError(MUCRoom * /*room*/, StanzaError error)
{
	printf("!!!!!!!!got an error: %d", error);
	if (error == StanzaErrorRegistrationRequired)
	{
		Q_EMIT joinGroupResultSignal(false);
		m_room->leave();
		delete m_room;
	}
}

void XmppGroup::handleMUCInfo(MUCRoom * /*room*/, int features, const std::string& name,
	const DataForm* infoForm)
{
	printf("features: %d, name: %s, form xml: %s\n",
		features, name.c_str(), infoForm->tag()->xml().c_str());
	string desc = infoForm->field("muc#roominfo_description")->value();
	/*if (m_info.id == name.c_str() || m_info.id == desc.c_str())
	{
		return;
	}*/
	m_info.name = name.c_str();
	m_info.description = desc.c_str();
}

void XmppGroup::handleMUCItems(MUCRoom * /*room*/, const Disco::ItemList& items)
{
	Disco::ItemList::const_iterator it = items.begin();
	for (; it != items.end(); ++it)
	{
		printf("%s -- %s is an item here\n", (*it)->jid().full().c_str(), (*it)->name().c_str());
	}
}

void XmppGroup::handleMUCInviteDecline(MUCRoom * /*room*/, const JID& invitee, const std::string& reason)
{
	printf("Invitee %s declined invitation. reason given: %s\n", invitee.full().c_str(), reason.c_str());
}

bool XmppGroup::handleMUCRoomCreation(MUCRoom *room)
{
	printf("room %s didn't exist, beeing created.\n", room->name().c_str());
	Q_EMIT createGroupResultSignal(m_info.id);
	return true;
}

void XmppGroup::handleMUCConfigList(MUCRoom* room, const MUCListItemList& items, MUCOperation operation)
{
	MUCListItemList::const_iterator it;
	if (operation == RequestOwnerList)
	{
		for (it = items.begin(); it != items.end(); it++)
		{
			printf("owner----------- %s\n", it->jid().username().c_str());
			m_owner = QString(it->jid().username().c_str()).append("@localhost");
			m_client->queryVCard(m_owner);
			break;
		}
	}
	else if (operation == RequestMemberList)
	{
		m_members.clear();
		for (it = items.begin(); it != items.end(); it++)
		{
			printf("member----------- %s\n", it->jid().username().c_str());
			m_members.append(QString(it->jid().username().c_str()).append("@localhost"));
			m_client->queryVCard(QString(it->jid().username().c_str()).append("@localhost"));
		}
	}
}

void XmppGroup::handleMUCConfigForm(MUCRoom* room, const DataForm& form)
{

}

void XmppGroup::handleMUCConfigResult(MUCRoom* room, bool success, MUCOperation operation)
{

}

void XmppGroup::handleMUCRequest(MUCRoom* room, const DataForm& form)
{

}

void XmppGroup::onContactFoundResult(int result, QVariant dataVar)
{
	if (result == 1)
	{
		UserInfo userInfo;
		userInfo = dataVar.value<UserInfo>();
		m_membersInfoMap.insert(userInfo.jid, userInfo);
	}
}