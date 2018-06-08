#include "stwbinvitefriend.h"

using namespace tahiti;

// ------------------------ STWBInviteFriendItem ------------------------
STWBInviteFriendItem::STWBInviteFriendItem(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);
}

STWBInviteFriendItem::~STWBInviteFriendItem()
{

}

void STWBInviteFriendItem::setUserInfo(UserInfo userInfo)
{
	m_userInfo = userInfo;

	if (userInfo.userName.isEmpty())
	{
		ui.lblFriendName->setText(userInfo.jid);
	}
	else
	{
		ui.lblFriendName->setText(userInfo.userName);
	}

	QString path = userInfo.photoPath;
	if (!QFile::exists(path))
	{
		path = ":/SoftTerminal/images/account.png";
	}
	QImage* image = new QImage(path);
	ui.lblFriendPic->setPixmap(QPixmap::fromImage(*image).scaled(36, 36));
}

UserInfo STWBInviteFriendItem::getUserInfo()
{
	return m_userInfo;
}

void STWBInviteFriendItem::on_cbCheck_clicked(bool checked)
{
	if (checked)
	{
		Q_EMIT inviteFriendSignal(m_userInfo.jid);
	}
	else
	{
		Q_EMIT uninviteFriendSignal(m_userInfo.jid);
	}
}

// ------------------------ STWBInviteFriend ------------------------
STWBInviteFriend::STWBInviteFriend(XmppClient* client, QWidget * parent)
	: m_xmppClient(client), QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
}

STWBInviteFriend::~STWBInviteFriend()
{
}

void STWBInviteFriend::initFriendList()
{
	ui.lwFriendList->clear();

	STWBInviteFriendItem* friendItem;
	QListWidgetItem* item;

	QList<UserInfo> friendList = m_xmppClient->getRoster();
	QList<UserInfo>::const_iterator it;
	for (it = friendList.constBegin(); it != friendList.constEnd(); it++)
	{
		friendItem = new STWBInviteFriendItem();
		friendItem->setUserInfo(*it);
		connect(friendItem, SIGNAL(inviteFriendSignal(QString)), this, SLOT(inviteFriendSlot(QString)));
		connect(friendItem, SIGNAL(uninviteFriendSignal(QString)), this, SLOT(uninviteFriendSlot(QString)));
		item = new QListWidgetItem();
		ui.lwFriendList->addItem(item);
		ui.lwFriendList->setItemWidget(item, friendItem);
	}
	m_inviteList.clear();
}

void STWBInviteFriend::inviteFriendSlot(QString jid)
{
	m_inviteList.append(jid);
}

void STWBInviteFriend::uninviteFriendSlot(QString jid)
{
	m_inviteList.removeOne(jid);
}

void STWBInviteFriend::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}
}

void STWBInviteFriend::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint pos = this->pos() + movePoint;
		m_startMovePos = event->globalPos();
		this->move(pos.x(), pos.y());
	}
}

void STWBInviteFriend::mouseReleaseEvent(QMouseEvent* event)
{
	m_isPressed = false;
}

void STWBInviteFriend::on_pbOK_clicked()
{
}

void STWBInviteFriend::on_pbCancel_clicked()
{
}

void STWBInviteFriend::on_pbClose_clicked()
{
	Q_EMIT closeInviteFriend();
}

void STWBInviteFriend::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Escape:
		break;
	default:
		QDialog::keyPressEvent(event);
	}
}
