#include "xmppregister.h"

using namespace std;
using namespace gloox;
using namespace tahiti;

XmppRegister::XmppRegister()
{
}

XmppRegister::~XmppRegister()
{
}

/* 账号注册 */
void XmppRegister::registAccount(QString user, QString passwd, QString server, QString port)
{
	m_xmppUser = user;
	m_xmppPasswd = passwd;
	m_xmppServerIP = server;
	m_xmppServerPort = port.toInt();

	m_registClient = new Client(server.toUtf8().constData());
	m_registClient->setPort(m_xmppServerPort);
	m_registClient->disableRoster();
	m_registClient->registerConnectionListener(this);

	m_reg = new Registration(m_registClient);
	m_reg->registerRegistrationHandler(this);

	m_registClient->logInstance().registerLogHandler(LogLevelDebug, LogAreaAll, this);
	m_registClient->connect();

	delete(m_reg);
	delete(m_registClient);
}

void XmppRegister::handleRegistrationFields(const JID& /*from*/, int fields, std::string instructions)
{
	printf("fields: %d\ninstructions: %s\n", fields, instructions.c_str());
	RegistrationFields vals;
	vals.username = m_xmppUser.toUtf8().constData();
	vals.password = m_xmppPasswd.toUtf8().constData();
	m_reg->createAccount(fields, vals);
}

void XmppRegister::handleRegistrationResult(const JID& /*from*/, RegistrationResult result)
{
	printf("result: %d\n", result);
	m_registClient->disconnect();
	Q_EMIT registResult(true);
}

void XmppRegister::handleAlreadyRegistered(const JID& /*from*/)
{
	printf("the account already exists.\n");
	Q_EMIT registResult(false);
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

/* 连接上 */
void XmppRegister::onConnect()
{
	TAHITI_INFO("connect server success!");
	m_reg->fetchRegistrationFields();
}

/* 连接丢失 */
void XmppRegister::onDisconnect(ConnectionError e)
{
	TAHITI_INFO("disconnect server!");
}

bool XmppRegister::onTLSConnect(const CertInfo& info)
{
	return true;
}
