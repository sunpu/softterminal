#include "stcontactdetail.h"

using namespace tahiti;

STContactDetail::STContactDetail(QWidget* parent) : QWidget(parent)
{

	ui.setupUi(this);
}

STContactDetail::~STContactDetail()
{

}

void STContactDetail::clearContactDetail()
{
	ui.lblJid->clear();
	ui.lblName->clear();
	ui.lblPic->clear();
}

void STContactDetail::setContactDetail(UserInfo userInfo)
{
	m_userInfo = userInfo;

	clearContactDetail();

	ui.lblJid->setText(userInfo.jid);
	ui.lblName->setText(userInfo.userName);
	ui.lblTelephone->setText(userInfo.telephone);
	ui.lblEmail->setText(userInfo.email);
	ui.lblDescription->setText(userInfo.description);

	QString path = userInfo.photoPath;
	if (path.size() == 0)
	{
		path = ":/SoftTerminal/images/account.png";
	}
	QImage* image = new QImage(path);
	ui.lblPic->setPixmap(QPixmap::fromImage(*image).scaled(80, 80));

	ui.widButton->setVisible(true);
	ui.widText->setVisible(false);
	ui.tabWidget->setCurrentIndex(0);
}

void STContactDetail::on_pbSendMessage_clicked()
{
	Q_EMIT openChatDetail(m_userInfo.jid);
}

void STContactDetail::on_pbDeleteFriend_clicked()
{
	ui.widButton->setVisible(false);
	ui.widText->setVisible(true);
	Q_EMIT deleteFriend(m_userInfo.jid);
}
