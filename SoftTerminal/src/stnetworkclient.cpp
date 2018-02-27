#include "STNetworkClient.h"

using namespace tahiti;

STNetworkClient::STNetworkClient(QObject * parent) : QObject(parent)
{
	m_tcpSocket = NULL;
}

STNetworkClient::~STNetworkClient()
{

}
void STNetworkClient::connectServer(QString ip, QString port)
{
	m_tcpSocket = new QTcpSocket(this); // 申请堆空间有TCP发送和接受操作
	m_tcpIp = ip;
	m_tcpPort = port;
	m_tcpSocket->connectToHost(m_tcpIp, m_tcpPort.toInt()); // 连接主机
	connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this,
		SLOT(displayError(QAbstractSocket::SocketError))); // 错误连接
	connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(connectUpdate())); // 更新连接之后按钮的使能
	connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readMessage())); // 读取信息的连接
}

void STNetworkClient::disconnectServer()
{
	m_tcpSocket->disconnect();
	delete(m_tcpSocket);
	m_tcpSocket = NULL;
}

void STNetworkClient::sendMessage(QString msg)
{
	m_tcpSocket->write("#*#");
	m_tcpSocket->write(msg.toUtf8());
	m_tcpSocket->write("@%@");
}

void STNetworkClient::readMessage()
{
	QString data = m_tcpSocket->readAll();

	if (data.endsWith("@%@"))
	{
		if (m_bigData.size() > 0)
		{
			data = m_bigData + data;
			m_bigData.clear();
		}
		data.replace("@%@", "");
		QStringList datas = data.split("#*#");
		for (int i = 0; i < datas.size(); i++)
		{
			if (datas[i].size() == 0)
			{
				continue;
			}
			Q_EMIT processMessage(datas[i]);
		}
	}
	else
	{
		m_bigData = m_bigData + data;
	}
}

void STNetworkClient::displayError(QAbstractSocket::SocketError e)
{
	printf("error:%d", e);
}

void STNetworkClient::connectUpdate()
{
	printf("connected");
	Q_EMIT onConnect();
}

void STNetworkClient::disconnectUpdate()
{
	printf("disconnected");
}
