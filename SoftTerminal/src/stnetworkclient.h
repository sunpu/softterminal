#ifndef STNETWORKCLIENT_H
#define STNETWORKCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <QThread>
#include <QMetaType>

namespace tahiti
{
	class STNetworkClient : public QObject
	{
		Q_OBJECT

	public:
		STNetworkClient(QObject * parent = 0);
		~STNetworkClient();
		void connectServer(QString ip, QString port);
		void disconnectServer();
		void sendMessage(QString data);
	private:
		private Q_SLOTS:
		void readMessage();
		void displayError(QAbstractSocket::SocketError);
		void connectUpdate();
		void disconnectUpdate();
	Q_SIGNALS:
		void onConnect();
		void processMessage(QString message);
	private:
		QTcpSocket* m_tcpSocket;//直接建立TCP套接字类
		QString m_tcpIp;//存储IP地址
		QString m_tcpPort;//存储端口地址
		QString m_bigData;

	};
}

#endif
