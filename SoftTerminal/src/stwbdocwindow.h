#ifndef _STWBDOCWINDOW_H_
#define _STWBDOCWINDOW_H_

#include <QDialog>
#include <QGraphicsView>
#include <QMouseEvent>
#include "ui_STWBDocWindow.h"
#include "stwbview.h"
#include "stnetworkclient.h"

namespace tahiti
{
	class STWBDocWindow : public QDialog
	{
		Q_OBJECT

	public:
		STWBDocWindow(STNetworkClient* network, int index, QWidget * parent = 0);
		~STWBDocWindow();
		int getDocWindowIndex() { return m_index; }
		public Q_SLOTS:
		void on_pbMaximum_clicked();
		void on_pbNormal_clicked();
		void on_pbClose_clicked();
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

	private:
		Ui::STWBDocWindowClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;
		STNetworkClient* m_network;
		STWBView* m_view;
		int m_index;
	};
}
#endif
