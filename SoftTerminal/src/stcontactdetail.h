#ifndef _STCONTACTDETAIL_H_
#define _STCONTACTDETAIL_H_

#include <QWidget>
#include "ui_STContactDetail.h"
#include "stcommon.h"
#include "stconfirm.h"

using namespace tahiti;

namespace tahiti
{
	class STContactDetail : public QWidget
	{
		Q_OBJECT

	public:
		STContactDetail(QWidget* parent = 0);
		~STContactDetail();
		void clearContactDetail();
		void setContactDetail(UserInfo userInfo);
	private:
		void confirmDeleteFriend();
		public Q_SLOTS:
		void on_pbSendMessage_clicked();
		void on_pbDeleteFriend_clicked();
		private Q_SLOTS:
		void handleConfirmOK();
	Q_SIGNALS:
		void openChatDetail(QString jid);
		void deleteFriend(QString jid);
	private:
		Ui::STContactDetailClass ui;
		UserInfo m_userInfo;
		STConfirm* m_confirm;
		QWidget* m_main;
	};
}
#endif
