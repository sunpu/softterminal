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
		void createClient(QString role);
		void createCourse(QString courseID);
		void joinCourse(QString courseID);
		void setClientAuthority(QString editable);
		void drawRealtimePenItem(QString penColor, int penThickness, QVector<QPoint> points);
		void drawPenItem(QString penColor, int penThickness, QVector<QPoint> points, int itemID);
		void drawTextItem(QString textColor, int textSize, QString content, QPoint position, int itemID);
		void moveItem(QPoint position, int itemID);
		void deleteItems(QList<int> itemIDs);
	private:
		void processMessage(QString message);
		private Q_SLOTS:
		void readMessage();
		void displayError(QAbstractSocket::SocketError);
		void connectUpdate();
		void disconnectUpdate();
	Q_SIGNALS:
		void onConnect();
		void processCloudFileMessage(QString message);
		void drawRemoteRealtimePen(QString color, int thickness, QVector<QPoint> points);
		void drawRemotePenItem(QString color, int thickness, QVector<QPoint> points, int itemID);
		void drawRemoteTextItem(QString color, int size, QString content, QPoint pos, int itemID);
		void moveRemoteItems(QPoint point, int itemID);
		void deleteRemoteItems(QList<int> itemIDs);
		void editableAuthority(QString editable);
	private:
		QTcpSocket* m_tcpSocket;//直接建立TCP套接字类
		QString m_tcpIp;//存储IP地址
		QString m_tcpPort;//存储端口地址
		QString m_bigData;

	};
}

#endif
