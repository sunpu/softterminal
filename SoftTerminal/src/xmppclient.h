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

Q_DECLARE_METATYPE(UserInfo)

namespace tahiti
{
	class CEventHandler :public EventHandler
	{
	public:
		CEventHandler();
		virtual ~CEventHandler();
		virtual void handleEvent(const Event& event);
		void resetHeartBeatCount();
		void increaceHeartBeatCount();
		int getHeartBeatCount() { return m_count; }
	private:
		void decreaceHeartBeatCount();
	private:
		int m_count;
	};

	class XmppClient : public QObject, RegistrationHandler, RosterListener, ConnectionListener,
		MessageSessionHandler, LogHandler, MessageEventHandler, MessageHandler, VCardHandler, MUCRoomHandler, DiscoHandler
	{
		Q_OBJECT
	public:
		XmppClient();
		virtual ~XmppClient();
		static void* longConnectProc(void* args);
		static void* keepaliveProc(void* args);
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
		/*string deviceName(string deviceName);
		string xmppAccount(string xmppAccount);
		string xmppStatus();
		string xmppLogin();
		string xmppLogout();
		string xmppKeepalive(string xmppKeepalive);*/
		int getKeepaliveInterval();
		int getKeepaliveCount();
		Presence::PresenceType getXmppStatus();
		void setXmppStatus(Presence::PresenceType status);
		bool isNeedLogin() { return m_needLogin; }
		string getXmppID() { return m_xmppID; }
		void sendMsg(QString dest, QString msg);
	Q_SIGNALS:
		void loginResult(bool result);
		void contactFoundResult(int result, QVariant dataVar);
		void showMessage(QString jid, QString msg);
	private:
		void notifyMyInfo();
		void createNewClient();
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
		Client* getClient();
	private:
		Client* m_client;
		VCardManager* m_vManager;
		pthread_t m_tidConnect;
		pthread_t m_tidKeepalive;
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
	};
}
#endif
