#include "stcontactdetail.h"

using namespace tahiti;

STContactDetail::STContactDetail(QWidget* parent)
	: QWidget(parent), m_main(parent)
{
	ui.setupUi(this);

	m_confirm = new STConfirm(this);
	connect(m_confirm, SIGNAL(confirmOK()), this, SLOT(handleConfirmOK()));
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
	if (!QFile::exists(path))
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
	confirmDeleteFriend();
}

void STContactDetail::confirmDeleteFriend()
{
	m_confirm->setText(QStringLiteral("您是否确定删除该好友？"));
	int x = m_main->pos().x() + (m_main->width() - m_confirm->width()) / 2;
	int y = m_main->pos().y() + (m_main->height() - m_confirm->height()) / 2;
	m_confirm->move(QPoint(x, y));
	m_confirm->exec();
}

void STContactDetail::handleConfirmOK()
{
	ui.widButton->setVisible(false);
	ui.widText->setVisible(true);
	Q_EMIT deleteFriend(m_userInfo.jid);
}
