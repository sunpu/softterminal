#ifndef _STWBROSTER_H_
#define _STWBROSTER_H_

#include <QDialog>
#include <QMouseEvent>
#include "xmppclient.h"
#include "ui_STWBRoster.h"

namespace tahiti
{
	class STWBRoster : public QDialog
	{
		Q_OBJECT

	public:
		STWBRoster(XmppClient* client, QWidget * parent = 0);
		~STWBRoster();
		void initRoster();
		public Q_SLOTS:
		void on_pbClose_clicked();
		private Q_SLOTS:
		void clearSearchInput();
		void onTextChanged();
	Q_SIGNALS:
		void closeRoster();
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
		virtual void keyPressEvent(QKeyEvent *event);
		virtual void resizeEvent(QResizeEvent *event);
		bool eventFilter(QObject* obj, QEvent* e);
	private:
		void resizeHeaders();
	private:
		Ui::STWBRosterClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;
		XmppClient* m_xmppClient;
		QPushButton* m_clearBtn;
	};
}
#endif
