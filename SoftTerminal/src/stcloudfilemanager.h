#ifndef _STCLOUDFILEMANAGER_H_
#define _STCLOUDFILEMANAGER_H_

#include <QWidget>
#include <QDialog>
#include <QCheckBox>
#include <QLabel>
#include <QFileInfo>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <QMouseEvent>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QStandardPaths>
#include <QEvent>
#include <windows.h>
#include <Winuser.h>
#include "stnetworkclient.h"
#include "ui_STCloudFileManager.h"
#include "ui_STCloudSupport.h"
#include "ui_STCloudUploadFile.h"
#include "ui_STCloudNewFolder.h"
#include "ui_STCloudFolderView.h"

namespace tahiti
{
	typedef struct fileInfo
	{
		QString type;
		int id;
		QString name;
	} FileInfo;

	class STCloudSupport;
	class STCloudUploadFile;
	class STCloudNewFile;
	class STCloudFolderView;
	class STCloudFileManager : public QWidget
	{
		Q_OBJECT

	public:
		STCloudFileManager(QWidget * parent = 0);
		~STCloudFileManager();
		void initCloudFileView();
	protected:
		void setRowColor(int row, QColor color);
		virtual void resizeEvent(QResizeEvent *);
		public Q_SLOTS :
		void on_pbUpload_clicked();
		void on_pbNew_clicked();
		void on_pbCopy_clicked();
		void on_pbMove_clicked();
		void on_pbDel_clicked();
		void on_pbSupport_clicked();
		void changeRowColor(int row, int column);
		private Q_SLOTS :
		void onFileItemClicked();
		void onFolderClicked();
		void processMessage(QString);
		void handleNewFile(QString);
		void handleFolderView(QString, QString);
	private:
		void resizeHeaders();
		void refreshCurrentPageTable();
		void makeCurrentPageTable(QString data);
		void callCloudFolderView(QString data);
		void getCheckList();
		//QJsonArray getCurrentQJsonArray(QJsonArray itemArray);
	private:
		Ui::STCloudFileManagerClass ui;
		QColor defaultBkColor;
		int previousColorRow;
		QMap<QPushButton*, FileInfo> m_tableBtnInfo;
		QList<QString> m_folderList;
		QList<QPushButton*> m_folderBtnList;
		QMap<QPushButton*, FileInfo> m_folderBtnInfo;
		STNetworkClient* m_network;
		QString m_action;
		QList<QString> m_checkedList;
		STCloudSupport* m_support;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////
	class STCloudUploadClient : public QObject
	{
		Q_OBJECT

	public:
		STCloudUploadClient(QObject * parent = 0);
		~STCloudUploadClient();
		void uploadFile(QString destPath, QString file);
	private:
		void displayError(QAbstractSocket::SocketError);
		void connectServer(QString ip, QString port);
		void disconnectServer();
		private Q_SLOTS :
		void sendFileInfo();  //传送文件头信息
		void uploadFileData(qint64);  //传送文件内容
	Q_SIGNALS:
		void onUploadProcess(int percent);
		void onUploadFinished();
	private:
		QTcpSocket* m_tcpSocket;//直接建立TCP套接字类
		QString m_tcpIp;//存储IP地址
		QString m_tcpPort;//存储端口地址
		QByteArray m_outBlock;  //分次传  
		qint64 m_loadSize;  //每次发送数据的大小  
		qint64 m_byteToWrite;  //剩余数据大小  
		qint64 m_totalSize;  //文件总大小  
		QFile* m_localFile;
		QString m_destPath;
		QString m_file;
		bool m_startUploadFileData;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////
	class STCloudSupport : public QWidget
	{
		Q_OBJECT

	public:
		STCloudSupport(QWidget * parent = 0);
		~STCloudSupport();
	protected:
		bool event(QEvent* event);
	private:
		Ui::STCloudSupportClass ui;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////
	class STCloudUploadFile : public QDialog
	{
		Q_OBJECT

	public:
		STCloudUploadFile(QWidget * parent = 0);
		~STCloudUploadFile();
		public Q_SLOTS:
		void on_pbOK_clicked();
		void on_pbGoon_clicked();
		void on_pbCancel_clicked();
		void on_pbClose_clicked();
		void on_pbUpload_clicked();
		public Q_SLOTS:
		void onUploadFinished();
		void onUploadProcess(int percent);
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
	Q_SIGNALS:
		void confirmOK(QString);
	private:
		Ui::STCloudUploadFileClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;
		STCloudUploadClient* m_uploadClient;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////
	class STCloudNewFolder : public QDialog
	{
		Q_OBJECT

	public:
		STCloudNewFolder(QWidget * parent = 0);
		~STCloudNewFolder();
		public Q_SLOTS:
		void on_pbOK_clicked();
		void on_pbCancel_clicked();
		void on_pbClose_clicked();
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
	Q_SIGNALS:
		void confirmOK(QString);
	private:
		Ui::STCloudNewFolderClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////
	class STCloudFolderView : public QDialog
	{
		Q_OBJECT

	public:
		STCloudFolderView(QString action, QString folders, QWidget * parent = 0);
		~STCloudFolderView();
		public Q_SLOTS:
		void on_pbOK_clicked();
		void on_pbCancel_clicked();
		void on_pbClose_clicked();
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
	Q_SIGNALS:
		void confirmOK(QString, QString);
	private:
		void initFolderTree();
		void makeFolderSubTree(QStandardItem* item, QJsonArray arr, QString parentPath);
	private:
		Ui::STCloudFolderViewClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;
		QString m_action;
		QString m_folders;
	};
}
#endif
