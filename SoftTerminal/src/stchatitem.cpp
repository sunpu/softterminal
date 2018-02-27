#include "stchatitem.h"

using namespace tahiti;

STChatItem::STChatItem(QWidget* parent) : QWidget(parent)
{

	ui.setupUi(this);
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

UserInfo STChatItem::getUserInfo()
{
	return m_userInfo;
}
