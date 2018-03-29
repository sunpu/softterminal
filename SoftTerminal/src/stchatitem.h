#ifndef _STCHATITEM_H
#define _STCHATITEM_H

#include <QWidget>
#include "ui_STChatItem.h"
#include "stcommon.h"
#include "strecordmanager.h"

using namespace tahiti;

namespace tahiti
{
	class STChatItem : public QWidget
	{
		Q_OBJECT

	public:
		STChatItem(QWidget* parent = 0);
		~STChatItem();
		void setUserInfo(UserInfo userInfo);
		UserInfo getUserInfo();
		void updateMessage();
		void updateUnreadNum();
		void clearUnreadNum();
	private:
		Ui::STChatItemClass ui;
		UserInfo m_userInfo;
		int m_unreadNum;
	};
}
#endif
