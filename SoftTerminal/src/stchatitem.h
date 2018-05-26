#ifndef _STCHATITEM_H
#define _STCHATITEM_H

#include <QWidget>
#include "ui_STChatItem.h"
#include "stcommon.h"
#include "strecordmanager.h"
#include "xmppclient.h"

using namespace tahiti;

namespace tahiti
{
	class STChatItem : public QWidget
	{
		Q_OBJECT

	public:
		STChatItem(QWidget* parent = 0);
		~STChatItem();
		void setChatInfo(UserInfo userInfo, XmppGroup* group = NULL);
		UserInfo getUserInfo();
		void updateOthersMessage(RecordItem item);
		void updateUnreadNum();
		void clearUnreadNum();
		XmppGroup* getGroup();
	private:
		Ui::STChatItemClass ui;
		UserInfo m_userInfo;
		int m_unreadNum;
		XmppGroup* m_group;
	};
}
#endif
