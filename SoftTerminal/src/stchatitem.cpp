﻿#include "stchatitem.h"

using namespace tahiti;

STChatItem::STChatItem(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);
	m_unreadNum = 0;
	ui.lblUnreadNum->setVisible(false);
}

STChatItem::~STChatItem()
{

}

void STChatItem::setChatInfo(UserInfo userInfo, XmppGroup* group)
{
	m_userInfo = userInfo;
	m_group = group;

	ui.lblChatName->setText(userInfo.userName);

	QString path = userInfo.photoPath;
	if (path.size() == 0)
	{
		path = ":/SoftTerminal/images/account.png";
	}
	QImage* image = new QImage(path);
	ui.lblChatPic->setPixmap(QPixmap::fromImage(*image).scaled(36, 36));

	STRecordManager* recordManager = new STRecordManager(userInfo.jid);
	RecordItem recordItem = recordManager->getLastestRecordItem();

	QString time = recordItem.time;
	QString content = recordItem.content;
	ui.lblTime->setText(time);
	if (m_group)
	{
		QString name = m_group->getUserInfo(recordItem.jid).userName;
		if (name.isEmpty())
		{
			name = recordItem.jid;
		}
		if (!recordItem.content.isEmpty())
		{
			content = QString("%1:%2").arg(name, recordItem.content);
		}
	}
	ui.lblChatPreview->setText(content);
}

void STChatItem::updateOthersMessage(RecordItem item)
{
	STRecordManager* recordManager = new STRecordManager(m_userInfo.jid);
	recordManager->writeRecordItem(item);

	QString time = item.time;
	QString content = item.content;
	ui.lblTime->setText(time);
	if (m_group)
	{
		QString name = m_group->getUserInfo(item.jid).userName;
		if (name.isEmpty())
		{
			name = item.jid;
		}
		content = QString("%1:%2").arg(name, item.content);
	}
	ui.lblChatPreview->setText(content);
}

UserInfo STChatItem::getUserInfo()
{
	return m_userInfo;
}

XmppGroup* STChatItem::getGroup()
{
	return m_group;
}

void STChatItem::updateUnreadNum()
{
	m_unreadNum++;
	QString showText;
	if (m_unreadNum >= 100)
	{
		showText = "99+";
	}
	else
	{
		showText = QString::number(m_unreadNum);
	}
	ui.lblUnreadNum->setText(showText);
	ui.lblUnreadNum->setVisible(true);
}

void STChatItem::clearUnreadNum()
{
	m_unreadNum = 0;
	ui.lblUnreadNum->clear();
	ui.lblUnreadNum->setVisible(false);
}
