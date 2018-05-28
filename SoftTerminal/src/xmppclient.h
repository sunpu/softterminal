#ifndef _XMPPCLIENT_H_
#define _XMPPCLIENT_H_

#include <QObject>
#include <QString>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QThread>
#include <QFile>
#include <QDataStream>
#include <QList>
#include <QStandardPaths>
#include <QFileInfo>
#include <QDirIterator>
#include <QDateTime>
#include "gloox/macros.h"
#include "gloox/registration.h"
#include "gloox/vcardhandler.h"
#include "gloox/vcardmanager.h"
#include "gloox/vcard.h"
#include "gloox/stanza.h"

#include "gloox/client.h"
#include "gloox/connectionlistener.h"
#include "gloox/disco.h"
#include "gloox/stanza.h"
#include "gloox/gloox.h"
#include "gloox/lastactivity.h"
#include "gloox/rostermanager.h"
#include "gloox/connectiontcpclient.h"
#include "gloox/messagesessionhandler.h"
#include "gloox/messageeventhandler.h"
#include "gloox/messagehandler.h"
#include "gloox/message.h"
#include "gloox/messageeventfilter.h"
#include "gloox/loghandler.h"
#include "gloox/eventhandler.h"
#include "gloox/mucroomhandler.h"
#include "gloox/mucroom.h"
#include "gloox/dataform.h"
#include "logger.h"
#include "json/json.h"
#include "pthread/pthread.h"
#include "stcommon.h"
#include "stconfig.h"

using namespace std;
using namespace gloox;
using namespace tahiti;

#pragma comment(lib, "pthreadVC2.lib")

#define STR_EMPTY ""
#define DEV_TYPE "st"
#define JID_LEN 32
#define XMPP_STATUS_LEN 128
#define MSG_LEN 2048
#define SQL_LEN 2048

//Q_DECLARE_METATYPE(UserInfo)

namespace tahiti
{
	class XmppGroup;
	class XmppClient : public QObject, RegistrationHandler, RosterListener, ConnectionListener,
		MessageSessionHandler, LogHandler, MessageEventHandler, MessageHandler, VCardHandler,
		DiscoHandler
	{
		Q_OBJECT
	public:
		XmppClient();
		virtual ~XmppClient();
		static void* longConnectProc(void* args);
		static void* refreshSignalProc(void* args);
		static void* loadOfflineMsgProc(void* args);
		void setXmppAccount(QString user, QString passwd, QString server, QString port);
		void run();
		void login();
		void logout();
		void queryRoster();
		QList<UserInfo> getRoster();
		UserInfo getSelfInfo();
		void modifyPasswd(QString password);
		void queryVCard(QString jid);
		void modifyVCard(UserInfo userInfo);
		void modifySelfPic(QString picFile);
		bool subscribeOther(QString jid);
		bool unsubscribeOther(QString jid);
		int getKeepaliveInterval();
		int getKeepaliveCount();
		Presence::PresenceType getXmppStatus();
		void setXmppStatus(Presence::PresenceType status);
		bool isNeedLogin() { return m_needLogin; }
		string getXmppID() { return m_xmppID; }
		void sendMsg(QString dest, QString msg);
		QList<QString> getSubscriptionRequests() { return m_subscriptionRequestList; };
		void refreshSignal();
		Client* getClient();
		QList<XmppGroup*> getGroups() { return m_mucGroupList; };
		void createGroup(GroupInfo info, QList<QString> members);
		void removeGroup(QString id);
		private Q_SLOTS:
		void ackSubscriptionRequest(QString jid, bool ack);
		void joinGroupResultSlot(bool result);
		void createGroupResultSlot(QString id);
		void processOfflineMsg();
	Q_SIGNALS:
		void loginResult(bool result);
		void contactFoundResult(int result, QVariant dataVar);
		void showMessage(QString jid, QString msg);
		void subscriptionRequest(QString jid);
		void refreshContactSignal();
		void joinGroupResultSignal(bool result);
		void createGroupResultSignal(QString id);
	private:
		void notifyMyInfo();
		void createNewClient();
		void callRefreshSignalProc();
		virtual void onConnect();
		virtual void onDisconnect(ConnectionError e);
		virtual bool onTLSConnect(const CertInfo& info);
		virtual void handleMessageEvent(const JID& from, MessageEventType event);
		virtual void handleMessageSession(MessageSession* session);
		virtual void handleMessage(const Message& msg, MessageSession* session);
		virtual void handleLog(gloox::LogLevel level, LogArea area, const string& message);
		virtual void handleItemSubscribed(const JID& jid);
		virtual void handleItemAdded(const JID& jid);
		virtual void handleItemUnsubscribed(const JID& jid);
		virtual void handleItemRemoved(const JID& jid);
		virtual void handleItemUpdated(const JID& jid);
		virtual void handleRoster(const Roster& roster);
		virtual void handleRosterError(const IQ& iq);
		virtual void handleRosterPresence(const RosterItem& item, const std::string& resource,
			Presence::PresenceType presence, const std::string& msg);
		virtual void handleSelfPresence(const RosterItem& item, const std::string& resource,
			Presence::PresenceType presence, const std::string& msg);
		virtual bool handleSubscriptionRequest(const JID& jid, const std::string& msg);
		virtual bool handleUnsubscriptionRequest(const JID& jid, const std::string& msg);
		virtual void handleNonrosterPresence(const Presence& presence);
		virtual void handleRegistrationFields(const JID& /*from*/, int fields, std::string instructions);
		virtual void handleRegistrationResult(const JID& /*from*/, RegistrationResult result);
		virtual void handleAlreadyRegistered(const JID& /*from*/);
		virtual void handleDataForm(const JID& /*from*/, const DataForm& /*form*/);
		virtual void handleOOB(const JID& /*from*/, const OOB& oob);
		virtual void handleVCard(const JID& jid, const VCard *v);
		virtual void handleVCardResult(VCardContext context, const JID& jid,
			StanzaError se = StanzaErrorUndefined);
		virtual void handleDiscoItems(const JID& /*from*/, const Disco::Items& items, int context);
		virtual void handleDiscoInfo(const JID& /*from*/, const Disco::Info& info, int context);
		virtual void handleDiscoError(const JID& /*from*/, const Error* /*error*/, int context);
	private:
		Client* m_client;
		VCardManager* m_vManager;
		pthread_t m_tidConnect;
		pthread_t m_tidKeepalive;
		pthread_t m_tidRefreshSignal;
		pthread_t m_tidLoadOfflineMsg;
		//MessageSession* m_msgSession;
		MessageEventFilter* m_msgEventFilter;
		Presence::PresenceType m_xmppStatus;
		string m_deviceName;
		string m_xmppAccount;
		QString m_xmppUser;
		QString m_xmppPasswd;
		QString m_xmppServerIP;
		int m_xmppServerPort;
		string m_xmppID;
		QString m_xmppTempPasswd;
		string m_xmppKeepalive;
		int m_keepaliveInterval;
		int m_keepaliveCount;
		string m_macAddress;
		string m_ipAddress;
		map<int, string> m_responseMap;
		int m_requestId;
		bool m_needLogin;
		VCard* m_current_vcard;
		VCard* m_self_vcard;
		QList<UserInfo> m_friendList;
		QMap<QString, MessageSession*> m_sessionMap;
		UserInfo m_selfInfo;
		QList<QString> m_subscriptionRequestList;
		MessageSession* m_sess;
		QList<XmppGroup*> m_mucGroupList;
		bool m_isWorking;
		QMap<QString, QList<QString>> m_offlineMsgMap;
	};

	class XmppGroup : public QObject, MUCRoomHandler, MUCRoomConfigHandler
	{
		Q_OBJECT
	public:
		XmppGroup(XmppClient* client, QString nick);
		virtual ~XmppGroup();
		void remove();
		QString getOwner() { return m_owner; };
		QList<QString> getMembers() { return m_members; };
		QList<QString> getOnlines() { return m_onlines; };
		void setMembers(QList<QString> members);
		void setGroupInfo(GroupInfo info);
		void sendMsg(QString msg);
		GroupInfo getGroupInfo() { return m_info; };
		UserInfo getUserInfo(QString jid) { return m_membersInfoMap[jid]; };
	Q_SIGNALS:
		void joinGroupResultSignal(bool result);
		void createGroupResultSignal(QString id);
		void showGroupMessage(QString jid, QString user, QString msg);
		void refreshOnlineSignal();
		private Q_SLOTS:
		void onContactFoundResult(int result, QVariant dataVar);
	private:
		virtual void handleMUCParticipantPresence(MUCRoom * /*room*/, const MUCRoomParticipant participant,
			const Presence& presence);
		virtual void handleMUCMessage(MUCRoom* /*room*/, const Message& msg, bool priv);
		virtual void handleMUCSubject(MUCRoom * /*room*/, const std::string& nick, const std::string& subject);
		virtual void handleMUCError(MUCRoom * /*room*/, StanzaError error);
		virtual void handleMUCInfo(MUCRoom * /*room*/, int features, const std::string& name,
			const DataForm* infoForm);
		virtual void handleMUCItems(MUCRoom * /*room*/, const Disco::ItemList& items);
		virtual void handleMUCInviteDecline(MUCRoom * /*room*/, const JID& invitee, const std::string& reason);
		virtual bool handleMUCRoomCreation(MUCRoom *room);
		virtual void handleMUCConfigList(MUCRoom* room, const MUCListItemList& items, MUCOperation operation);
		virtual void handleMUCConfigForm(MUCRoom* room, const DataForm& form);
		virtual void handleMUCConfigResult(MUCRoom* room, bool success, MUCOperation operation);
		virtual void handleMUCRequest(MUCRoom* room, const DataForm& form);
	private:
		MUCRoom* m_room;
		QString m_owner;
		QList<QString> m_members;
		QList<QString> m_onlines;
		GroupInfo m_info;
		XmppClient* m_client;
		QMap<QString, UserInfo> m_membersInfoMap;
	};
}
#endif
