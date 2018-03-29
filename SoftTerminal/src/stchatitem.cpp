#include "stchatitem.h"

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

void STChatItem::setUserInfo(UserInfo userInfo)
{
	m_userInfo = userInfo;

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
	ui.lblChatPreview->setText(content);
}

void STChatItem::updateMessage()
{
	STRecordManager* recordManager = new STRecordManager(m_userInfo.jid);
	RecordItem recordItem = recordManager->getLastestRecordItem();

	QString time = recordItem.time;
	QString content = recordItem.content;
	ui.lblTime->setText(time);
	ui.lblChatPreview->setText(content);
}

UserInfo STChatItem::getUserInfo()
{
	return m_userInfo;
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
