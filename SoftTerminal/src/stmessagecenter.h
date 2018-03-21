#ifndef _STMESSAGECENTER_H_
#define _STMESSAGECENTER_H_

#include <QDialog>
#include <QMouseEvent>
#include "ui_STMessageCenter.h"
#include "ui_STMessageItem.h"
#include "xmppclient.h"

namespace tahiti
{
	class STMessageItem;
	class STMessageCenter : public QDialog
	{
		Q_OBJECT

	public:
		STMessageCenter(XmppClient* client, QWidget * parent = 0);
		~STMessageCenter();
		public Q_SLOTS:
		void on_pbClose_clicked();
		private Q_SLOTS:
		void handleSubscriptionRequest(QString);
	Q_SIGNALS:
		void refreshContactSignal();
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);

	private:
		Ui::STMessageCenterClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;
		XmppClient* m_xmppClient;
	};

	class STMessageItem : public QWidget
	{
		Q_OBJECT

	public:
		STMessageItem(XmppClient* client, QWidget * parent = 0);
		~STMessageItem();
		void setUserJid(QString jid);
		private Q_SLOTS:
		void onContactFoundResult(int result, QVariant dataVar);
		public Q_SLOTS:
		void on_pbAgree_clicked();
		void on_pbRefuse_clicked();
	Q_SIGNALS:
		void ackSubscriptionRequest(QString id, bool ack);
	private:
		Ui::STMessageItemClass ui;
		XmppClient* m_xmppClient;
		QString m_jid;
	};
}
#endif

