#ifndef _STWBCLOUDFILEVIEW_H_
#define _STWBCLOUDFILEVIEW_H_

#include <QWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QFileInfo>
#include <QMouseEvent>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include "stcommon.h"
#include "stconfig.h"
#include "stmessageclient.h"
#include "ui_STWBCloudFileView.h"

namespace tahiti
{
	class STWBCloudFileView : public QWidget
	{
		Q_OBJECT

	public:
		STWBCloudFileView(QWidget * parent = 0);
		~STWBCloudFileView();
		void initCloudFileView();
		public Q_SLOTS :
		void on_pbReload_clicked();
		void on_pbClose_clicked();
		private Q_SLOTS :
		void onFileItemClicked();
		void onFolderClicked();
		void processMessage(QString);
	Q_SIGNALS:
		void openCloudFileSignal(QString path);
		void closeCloudFileView();
	protected:
		virtual void resizeEvent(QResizeEvent *);
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
	private:
		void resizeHeaders();
		void refreshCurrentPageTable();
		void makeCurrentPageTable(QString data);
	private:
		Ui::STWBCloudFileViewClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;
		QMap<QPushButton*, FileInfo> m_tableBtnInfo;
		QList<QString> m_folderList;
		QList<QPushButton*> m_folderBtnList;
		QMap<QPushButton*, FileInfo> m_folderBtnInfo;
		STMessageClient* m_messageClient;
		QString m_action;
	};
}
#endif
