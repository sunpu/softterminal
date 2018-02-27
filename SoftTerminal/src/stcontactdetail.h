#ifndef _STCONTACTDETAIL_H_
#define _STCONTACTDETAIL_H_

#include <QWidget>
#include "ui_STContactDetail.h"
#include "stcommon.h"

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
		public Q_SLOTS:
		void on_pbSendMessage_clicked();
		void on_pbOpenClass_clicked();
	Q_SIGNALS:
		void openChatDetail(QString jid);
	private:
		Ui::STContactDetailClass ui;
		UserInfo m_userInfo;
	};
}
#endif
