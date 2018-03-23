#include "xmppregister.h"

using namespace std;
using namespace gloox;
using namespace tahiti;

XmppRegister::XmppRegister(QString server, QString port)
{
	m_xmppServerIP = server;
	m_xmppServerPort = port.toInt();
	m_checkMode = false;
	m_waitRemove = false;
	m_registClient = NULL;
	m_reg = NULL;
}

XmppRegister::~XmppRegister()
{
	if (m_registClient)
	{
		delete(m_registClient);
		m_registClient = NULL;
	}
	if (m_reg)
	{
		delete(m_reg);
		m_reg = NULL;
	}
}

/* ÕËºÅ×¢²á */
void XmppRegister::registAccount(QString user, QString passwd)
{
	m_xmppUser = user;
	m_xmppPasswd = passwd;

	m_registClient = new Client(m_xmppServerIP.toUtf8().constData());
	m_registClient->setPort(m_xmppServerPort);
	m_registClient->disableRoster();
	m_registClient->registerConnectionListener(this);

	m_reg = new Registration(m_registClient);
	m_reg->registerRegistrationHandler(this);

	m_registClient->logInstance().registerLogHandler(LogLevelDebug, LogAreaAll, this);
	m_registClient->connect();
}

/* ÕËºÅ²éÕÒ */
void XmppRegister::checkAccount(QString user, QString passwd)
{
	m_checkMode = true;
	m_waitRemove = false;
	registAccount(user, passwd);
}

/* ÕËºÅÉ¾³ý */
void XmppRegister::removeAccount(QString user, QString passwd)
{
	QString jidTmp = user + "@" + m_xmppServerIP;
	JID jid(jidTmp.toStdString());

	m_registClient = new Client(jid, passwd.toStdString(), m_xmppServerPort);
	m_registClient->disableRoster();
	m_registClient->registerConnectionListener(this);

	m_reg = new Registration(m_registClient);
	m_reg->registerRegistrationHandler(this);

	m_registClient->logInstance().registerLogHandler(LogLevelDebug, LogAreaAll, this);
	m_registClient->connect();
}

void XmppRegister::handleRegistrationFields(const JID& /*from*/, int fields, std::string instructions)
{
	printf("fields: %d\ninstructions: %s\n", fields, instructions.c_str());
	if (m_checkMode && m_waitRemove)
	{
		printf("removeAccount.\n");
		m_reg->removeAccount();
		m_checkMode = false;
	}
	else
	{
		printf("createAccount.\n");
		RegistrationFields vals;
		vals.username = m_xmppUser.toUtf8().constData();
		vals.password = m_xmppPasswd.toUtf8().constData();
		m_reg->createAccount(fields, vals);
	}
}

void XmppRegister::handleRegistrationResult(const JID& /*from*/, RegistrationResult result)
{
	printf("result: %d\n", result);
	m_registClient->disconnect();
	if (result == RegistrationResult::RegistrationSuccess)
	{
		printf("regist ok.\n");
		if (m_checkMode)
		{
			Q_EMIT checkResult(false);
			m_waitRemove = true;
			removeAccount(m_xmppUser, m_xmppPasswd);
		}
		else
		{
			Q_EMIT registResult(true);
		}
	}
	else
	{
		printf("regist ng.\n");
		if (m_checkMode)
		{
			Q_EMIT checkResult(true);
		}
		else
		{
			Q_EMIT registResult(false);
		}
	}
}

void XmppRegister::handleAlreadyRegistered(const JID& /*from*/)
{
	printf("the account already exists.\n");
	//Q_EMIT registResult(false);
}

void XmppRegister::handleDataForm(const JID& /*from*/, const DataForm& /*form*/)
{
	printf("datForm received\n");
}

void XmppRegister::handleOOB(const JID& /*from*/, const OOB& oob)
{
	printf("OOB registration requested. %s: %s\n", oob.desc().c_str(), oob.url().c_str());
}

void XmppRegister::handleLog(gloox::LogLevel level, LogArea area, const string& message)
{
	char buff[MSG_LEN] = { '\0' };
	sprintf(buff, "log: level: %d, area: %d, %s\n", level, area, message.c_str());
	TAHITI_INFO(buff);
}

/* Á¬½ÓÉÏ */
void XmppRegister::onConnect()
{
	TAHITI_INFO("connect server success!");
	m_reg->fetchRegistrationFields();
}

/* Á¬½Ó¶ªÊ§ */
void XmppRegister::onDisconnect(ConnectionError e)
{
	TAHITI_INFO("disconnect server!");
}

bool XmppRegister::onTLSConnect(const CertInfo& info)
{
	return true;
}
