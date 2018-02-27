#include "stcontactitem.h"

using namespace tahiti;

STContactItem::STContactItem(QWidget* parent) : QWidget(parent)
{

	ui.setupUi(this);
}

STContactItem::~STContactItem()
{

}

void STContactItem::setUserInfo(UserInfo userInfo)
{
	m_userInfo = userInfo;

	ui.lblContactName->setText(userInfo.userName);

	QString path = userInfo.photoPath;
	if (path.size() == 0)
	{
		path = ":/SoftTerminal/images/account.png";
	}
	QImage* image = new QImage(path);
	ui.lblContactPic->setPixmap(QPixmap::fromImage(*image).scaled(36, 36));
}

UserInfo STContactItem::getUserInfo()
{
	return m_userInfo;
}
