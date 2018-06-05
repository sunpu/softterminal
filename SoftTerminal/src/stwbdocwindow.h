#ifndef _STWBDOCWINDOW_H_
#define _STWBDOCWINDOW_H_

#include <QDialog>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPixmap>
#include <QScrollbar>
#include "JlCompress.h"
#include "ui_STWBDocWindow.h"
#include "stwbview.h"
#include "stconfig.h"
#include "stfileclient.h"

namespace tahiti
{
	class STWBDocWindow : public QDialog
	{
		Q_OBJECT

	public:
		STWBDocWindow(QString path, int index, QWidget * parent = 0);
		~STWBDocWindow();
		int getDocWindowIndex() { return m_index; }
		bool isMaximum() { return m_isMaximum; }
		public Q_SLOTS:
		void on_pbMaximum_clicked();
		void on_pbNormal_clicked();
		void on_pbClose_clicked();
		void on_pbPre_clicked();
		void on_pbNext_clicked();
		private Q_SLOTS:
		void setPenThickness(int thickness);
		void setPenColor(QString color);
		void setTextSize(int size);
		void setTextColor(QString color);
		void setActionMode(int mode);
		void deleteAction();
	Q_SIGNALS:
		void closeCloudFile(int index);
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
		bool eventFilter(QObject *obj, QEvent *e);
	private:
		void unzip();
		void loadFile();
		bool deleteDirectory(QString path);
		void showBefittingPic(QString picName);
		private Q_SLOTS:
		void onDownloadFinished();
	private:
		Ui::STWBDocWindowClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;
		STWBView* m_view;
		int m_index;
		QString m_path;
		QString m_docWindowPath;
		QString m_downloadPath;
		bool m_needUnzip;
		int m_currentPage;
		int m_totalPage;
		QString m_shortName;
		QString m_pngName;
		QRect m_normalRect;
		bool m_isMaximum;
		STFileClient* m_downloadClient;
	};
}
#endif
