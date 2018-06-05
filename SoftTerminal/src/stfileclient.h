#ifndef _STFILECLIENT_H
#define _STFILECLIENT_H

#include <QObject>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QFile>
#include "stcommon.h"
#include "stconfig.h"

namespace tahiti
{
	class STFileClient : public QObject
	{
		Q_OBJECT

	public:
		STFileClient(QObject * parent = 0);
		~STFileClient();
		void uploadFile(QString remotePath, QString localFilePath);
		void downloadFile(QString remotePath, QString localFilePath);
	private:
		void startUpload();
		void finishUpload();
		void startDownload();
		void finishDownload();
		private Q_SLOTS :
		void sendUploadInfo();
		void uploadFileData(qint64);
		void sendDownloadInfo();
		void downloadFileData();
		void displayError(QAbstractSocket::SocketError);
	Q_SIGNALS:
		void onUploadProcess(int percent);
		void onUploadFinished();
		void onDownloadFinished();
	private:
		QTcpSocket* m_tcpSocket;//直接建立TCP套接字类
		QByteArray m_outBlock;  //分次传
		qint64 m_loadSize;  //每次发送数据的大小
		qint64 m_byteToWrite;  //剩余数据大小
		qint64 m_totalSize;  //文件总大小
		bool m_startUploadFileData;
		QString m_remoteFilePath;
		qint64 m_bytesReceived;
		QFile* m_localFile;
		QString m_remotePath;
		QString m_localFilePath;
	};
}

#endif
