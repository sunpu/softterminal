#include "stmessageclient.h"

using namespace tahiti;

STMessageClient::STMessageClient(QObject * parent) : QObject(parent)
{
}

STMessageClient::~STMessageClient()
{
}

void* STMessageClient::connectProc(void* args)
{
	STMessageClient* client = (STMessageClient*)args;
	client->readMessage();
	return NULL;
}

void STMessageClient::subscribeMessage(QString filter)
{
	m_filter = filter;
	/*QString server = STConfig::getConfig("/xmpp/server");

	context_t context(1);
	m_subscriber = new socket_t(context, ZMQ_SUB);
	QString publisherUrl = QString("tcp://%1:%2").arg(server, QString::number(PUBLISHER_PORT));
	m_subscriber->connect(publisherUrl.toUtf8().constData());
	m_subscriber->setsockopt(ZMQ_SUBSCRIBE, filter.toUtf8().constData(), 0);*/

	pthread_create(&m_tidConnect, NULL, connectProc, this);
}

void STMessageClient::unsubscribeMessage()
{
	QString server = STConfig::getConfig("/xmpp/server");
	QString publisherUrl = QString("tcp://%1:%2").arg(server, QString::number(PUBLISHER_PORT));

	m_subscriber->disconnect(publisherUrl.toUtf8().constData());
	m_subscriber->close();
}

QString STMessageClient::sendMessage(QString msg)
{
	QString server = STConfig::getConfig("/xmpp/server");

	QString brokerUrl = QString("tcp://%1:%2").arg(server, QString::number(BROKER_PORT));
	mdcli session(brokerUrl.toUtf8().constData(), true);

	zmsg* request = new zmsg(msg.toUtf8().constData());
	zmsg* reply = session.send(ST_MESSAGE_SERVER, request);
	QString answer;
	if (reply)
	{
		answer = reply->body();
		processMessage(answer);
		delete reply;
	}
	return answer;
}

void STMessageClient::readMessage()
{
	QString server = STConfig::getConfig("/xmpp/server");

	context_t context(1);
	socket_t subscriber(context, ZMQ_SUB);
	QString publisherUrl = QString("tcp://%1:%2").arg(server, QString::number(PUBLISHER_PORT));
	subscriber.connect(publisherUrl.toUtf8().constData());
	subscriber.setsockopt(ZMQ_SUBSCRIBE, m_filter.toUtf8().constData(), 0);
	while (1)
	{
		string filter = s_recv(subscriber);
		string msg = s_recv(subscriber);
		cout << msg << endl;
		processMessage(msg.c_str());
	}
}

void STMessageClient::processMessage(QString message)
{
	if (message.startsWith("file_list") || message.startsWith("folder_list"))
	{
		Q_EMIT cloudFileMessageSignal(message);
	}
	else
	{
		Q_EMIT whiteBoardMessageSignal(message);
	}
}
