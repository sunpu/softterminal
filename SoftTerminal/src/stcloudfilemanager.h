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
#include "stcommon.h"
#include "stconfig.h"
#include "stmessageclient.h"
#include "stfileclient.h"
#include "ui_STCloudFileManager.h"
#include "ui_STCloudSupport.h"
#include "ui_STCloudUploadFile.h"
#include "ui_STCloudNewFolder.h"
#include "ui_STCloudFolderView.h"

namespace tahiti
{
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
		virtual void resizeEvent(QResizeEvent *);
		public Q_SLOTS :
		void on_pbUpload_clicked();
		void on_pbNew_clicked();
		void on_pbCopy_clicked();
		void on_pbMove_clicked();
		void on_pbDel_clicked();
		void on_pbSupport_clicked();
		void on_pbReload_clicked();
		private Q_SLOTS :
		void onFileItemClicked();
		void onFolderClicked();
		void processMessage(QString);
		void handleNewFile(QString);
		void handleFolderView(QString, QString);
		void onUploadFinished();
	private:
		void resizeHeaders();
		void refreshCurrentPageTable();
		void makeCurrentPageTable(QString data);
		void callCloudFolderView();
		void getCheckList();
		//QJsonArray getCurrentQJsonArray(QJsonArray itemArray);
	private:
		Ui::STCloudFileManagerClass ui;
		QMap<QPushButton*, FileInfo> m_tableBtnInfo;
		QList<QString> m_folderList;
		QList<QPushButton*> m_folderBtnList;
		QMap<QPushButton*, FileInfo> m_folderBtnInfo;
		STMessageClient* m_messageClient;
		QString m_action;
		QList<QString> m_checkedList;
		STCloudSupport* m_support;
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
		STCloudUploadFile(QString path, QWidget * parent = 0);
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
		void uploadFinished();
	private:
		Ui::STCloudUploadFileClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;
		STFileClient* m_uploadClient;
		QString m_path;
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
		STCloudFolderView(QString action, QWidget * parent = 0);
		~STCloudFolderView();
		public Q_SLOTS:
		void on_pbOK_clicked();
		void on_pbCancel_clicked();
		void on_pbClose_clicked();
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
		private Q_SLOTS:
		void processMessage(QString);
	Q_SIGNALS:
		void confirmOK(QString, QString);
	private:
		void initFolderTree(QString msg);
		void makeFolderSubTree(QStandardItem* item, QJsonArray arr, QString parentPath);
	private:
		Ui::STCloudFolderViewClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;
		QString m_action;
	};
}
#endif
