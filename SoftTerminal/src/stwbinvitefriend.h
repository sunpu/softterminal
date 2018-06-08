#ifndef _STWBINVITEFRIEND_H_
#define _STWBINVITEFRIEND_H_

#include <QDialog>
#include <QMouseEvent>
#include "stcommon.h"
#include "xmppclient.h"
#include "ui_STWBInviteFriend.h"
#include "ui_STWBInviteFriendItem.h"

namespace tahiti
{
	class STWBInviteFriendItem : public QWidget
	{
		Q_OBJECT

	public:
		STWBInviteFriendItem(QWidget* parent = 0);
		~STWBInviteFriendItem();
		void setUserInfo(UserInfo userInfo);
		UserInfo getUserInfo();
		public Q_SLOTS:
		void on_cbCheck_clicked(bool checked);		
	Q_SIGNALS:
		void inviteFriendSignal(QString jid);
		void uninviteFriendSignal(QString jid);
	private:
		Ui::STWBInviteFriendItemClass ui;
		UserInfo m_userInfo;
	};

	class STWBInviteFriend : public QDialog
	{
		Q_OBJECT

	public:
		STWBInviteFriend(XmppClient* client, QWidget * parent = 0);
		~STWBInviteFriend();
		void initFriendList();
		public Q_SLOTS :
		void on_pbOK_clicked();
		void on_pbCancel_clicked();
		void on_pbClose_clicked();
		private Q_SLOTS :
		void inviteFriendSlot(QString jid);
		void uninviteFriendSlot(QString jid);
	Q_SIGNALS:
		void closeInviteFriend();
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
		virtual void keyPressEvent(QKeyEvent *event);
	private:
		Ui::STWBInviteFriendClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;
		XmppClient* m_xmppClient;
		QList<QString> m_inviteList;
	};
}
#endif
