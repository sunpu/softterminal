#include "stmessagecenter.h"

using namespace tahiti;

STMessageCenter::STMessageCenter(XmppClient* client, QWidget * parent)
	: m_xmppClient(client), QDialog(parent)
{
	ui.setupUi(this);
	//setWindowModality(Qt::ApplicationModal);
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

	connect(m_xmppClient, SIGNAL(subscriptionRequest(QString)),
		this, SLOT(handleSubscriptionRequest(QString)));

	QList<QString> subscriptionRequests = m_xmppClient->getSubscriptionRequests();
	QList<QString>::Iterator iter;
	for (iter = subscriptionRequests.begin(); iter != subscriptionRequests.end(); iter++)
	{
		handleSubscriptionRequest(*iter);
	}
}

STMessageCenter::~STMessageCenter()
{

}

void STMessageCenter::on_pbClose_clicked()
{
	close();
}

void STMessageCenter::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}
}

void STMessageCenter::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint pos = this->pos() + movePoint;
		m_startMovePos = event->globalPos();
		this->move(pos.x(), pos.y());
	}
}

void STMessageCenter::mouseReleaseEvent(QMouseEvent* event)
{
	m_isPressed = false;
}

void STMessageCenter::handleSubscriptionRequest(QString jid)
{
	STMessageItem* messageItem = new STMessageItem(m_xmppClient);
	messageItem->setUserJid(jid);
	ui.widNewFriendMessage->layout()->addWidget(messageItem);
}

////////////////////////////////////////////////////////////////////////////////////////////////
STMessageItem::STMessageItem(XmppClient* client, QWidget * parent)
	: m_xmppClient(client), QWidget(parent)
{
	ui.setupUi(this);
	connect(m_xmppClient, SIGNAL(contactFoundResult(int, QVariant)), this, SLOT(onContactFoundResult(int, QVariant)));
	connect(this, SIGNAL(ackSubscriptionRequest(QString, bool)), m_xmppClient, SLOT(ackSubscriptionRequest(QString, bool)));

	ui.pbDelete->setVisible(false);
}

STMessageItem::~STMessageItem()
{

}

void STMessageItem::setUserJid(QString jid)
{
	if (!jid.endsWith("@localhost"))
	{
		jid.append("@localhost");
	}
	m_jid = jid;
	m_xmppClient->queryVCard(jid);
}

void STMessageItem::onContactFoundResult(int result, QVariant dataVar)
{
	if (result == 0)
	{
		ui.lblName->setText(m_jid);
		ui.lblMessage->setText(QStringLiteral("您好，我是") + m_jid);
	}
	else
	{
		UserInfo userInfo;
		userInfo = dataVar.value<UserInfo>();
		QString userName = userInfo.userName;
		if (userName.size() == 0)
		{
			userName = m_jid;
		}

		ui.lblName->setText(userName);
		ui.lblMessage->setText(QStringLiteral("您好，我是") + userName);

		QString path = userInfo.photoPath;
		if (path.size() == 0)
		{
			path = ":/SoftTerminal/images/account.png";
		}
		QImage* image = new QImage(path);
		ui.lblPic->setPixmap(QPixmap::fromImage(*image).scaled(36, 36));
	}
}

void STMessageItem::on_pbAgree_clicked()
{
	Q_EMIT ackSubscriptionRequest(m_jid, true);
	setVisible(false);
}

void STMessageItem::on_pbRefuse_clicked()
{
	Q_EMIT ackSubscriptionRequest(m_jid, false);
	setVisible(false);
}
