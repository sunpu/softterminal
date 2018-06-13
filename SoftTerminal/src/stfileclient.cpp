#include "stfileclient.h"

using namespace tahiti;

STFileClient::STFileClient(QObject * parent) : QObject(parent)
{
	m_tcpSocket = NULL;
}

STFileClient::~STFileClient()
{

}

void STFileClient::startUpload()
{
	delete(m_tcpSocket);
	m_tcpSocket = NULL;
	m_tcpSocket = new QTcpSocket(this);
	QString server = STConfig::getConfig("/xmpp/server");
	m_tcpSocket->connectToHost(server, FILE_SERVER_PORT);

	connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this,
		SLOT(displayError(QAbstractSocket::SocketError)));
	connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(sendUploadInfo()));  //当连接成功时，就开始传送文件  
	connect(m_tcpSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(uploadFileData(qint64)));
}

void STFileClient::finishUpload()
{
	disconnect(m_tcpSocket);
	m_tcpSocket->disconnect();
}

void STFileClient::displayError(QAbstractSocket::SocketError e)
{
	printf("error:%d", e);
}

void STFileClient::uploadFile(QString remotePath, QString localFilePath)
{
	m_remotePath = remotePath;
	m_localFilePath = localFilePath;

	m_loadSize = 0;
	m_byteToWrite = 0;
	m_totalSize = 0;
	m_startUploadFileData = false;
	m_outBlock.clear();

	m_localFile = new QFile(m_localFilePath);
	m_localFile->open(QFile::ReadOnly);

	m_byteToWrite = m_localFile->size();  //剩余数据的大小  
	m_totalSize = m_localFile->size();

	m_loadSize = 512 * 1024;  //每次发送数据的大小  

	startUpload();
}

void STFileClient::sendUploadInfo()
{
	QString destFilePath = m_remotePath + "/"
		+ m_localFilePath.right(m_localFilePath.size() - m_localFilePath.lastIndexOf('/') - 1);
	m_tcpSocket->write("#*#" + destFilePath.toUtf8() + "@%@");
}

void STFileClient::uploadFileData(qint64 numBytes)  //开始发送文件内容
{
	if (m_byteToWrite == 0)
	{
		finishUpload();
		return;
	}
	if (!m_startUploadFileData)
	{
		numBytes = 0;
		m_startUploadFileData = true;
	}
	m_byteToWrite -= numBytes;  //剩余数据大小
	m_outBlock = m_localFile->read(qMin(m_byteToWrite, m_loadSize));
	m_tcpSocket->write(m_outBlock);

	if (m_byteToWrite == 0)
	{
		m_tcpSocket->write("#*#finish@%@");
		m_localFile->close();
		//发送完毕
		Q_EMIT onUploadFinished();
	}
	else
	{
		int percent = (m_totalSize - m_byteToWrite) * 100 / m_totalSize;
		Q_EMIT onUploadProcess(percent);
	}
}


void STFileClient::downloadFile(QString remotePath, QString localFilePath)
{
	m_remoteFilePath = remotePath;
	m_localFilePath = localFilePath;
	m_localFile = NULL;

	startDownload();
}

void STFileClient::startDownload()
{
	delete(m_tcpSocket);
	m_tcpSocket = NULL;
	m_tcpSocket = new QTcpSocket(this);

	connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this,
		SLOT(displayError(QAbstractSocket::SocketError)));
	connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(sendDownloadInfo()));  //当连接成功时，就开始传送文件
	connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(downloadFileData()));

	QString server = STConfig::getConfig("/xmpp/server");
	m_tcpSocket->connectToHost(server, FILE_SERVER_PORT);
}

void STFileClient::finishDownload()
{
	disconnect(m_tcpSocket);
	m_tcpSocket->disconnect();
}

void STFileClient::sendDownloadInfo()
{
	m_bytesReceived = 0;
	m_tcpSocket->write("#*#download#" + m_remoteFilePath.toUtf8() + "@%@");
}

void STFileClient::downloadFileData()
{
	if (m_localFile == NULL)
	{
		m_localFile = new QFile(m_localFilePath);
		m_localFile->open(QIODevice::WriteOnly | QIODevice::Append);
	}
	else
	{
		m_bytesReceived += m_tcpSocket->bytesAvailable();
		QByteArray data = m_tcpSocket->readAll();
		// printf("%lld %d\n", bytesReceived, data.size());
		if (data.endsWith("#*#finish@%@"))
		{
			data = data.replace("#*#finish@%@", "");
			m_localFile->write(data, data.size());
			m_localFile->close();
			//发送完毕
			Q_EMIT onDownloadFinished();
		}
		else
		{
			m_localFile->write(data, data.size());
		}
		data.resize(0);
	}
}
