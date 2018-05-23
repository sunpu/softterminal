#include "stcontactitem.h"

using namespace tahiti;

STContactItem::STContactItem(bool littleMode, bool showAddBtn, bool showDeleteBtn, QWidget* parent)
	: QWidget(parent), m_littleMode(littleMode)
{
	ui.setupUi(this);

	ui.pbAdd->setVisible(showAddBtn);
	ui.pbDelete->setVisible(showDeleteBtn);

	if (m_littleMode)
	{
		setFixedHeight(30);
		ui.lblContactPic->setFixedSize(24, 24);
	}
}

STContactItem::~STContactItem()
{

}

void STContactItem::setUserInfo(UserInfo userInfo)
{
	m_userInfo = userInfo;

	if (userInfo.userName.isEmpty())
	{
		ui.lblContactName->setText(userInfo.jid);
	}
	else
	{
		ui.lblContactName->setText(userInfo.userName);
	}

	QString path = userInfo.photoPath;
	if (path.size() == 0)
	{
		path = ":/SoftTerminal/images/account.png";
	}
	QImage* image = new QImage(path);

	if (m_littleMode)
	{
		ui.lblContactPic->setPixmap(QPixmap::fromImage(*image).scaled(24, 24));
	}
	else
	{
		ui.lblContactPic->setPixmap(QPixmap::fromImage(*image).scaled(36, 36));
	}
}

UserInfo STContactItem::getUserInfo()
{
	return m_userInfo;
}

void STContactItem::on_pbAdd_clicked()
{
	Q_EMIT addFriendSignal(m_userInfo);
}

void STContactItem::on_pbDelete_clicked()
{
	Q_EMIT deleteMemberSignal(m_userInfo);
}
