#include "stcontactaddnew.h"

using namespace tahiti;

STContactAddNew::STContactAddNew(XmppClient* client) : m_xmppClient(client)
{

	ui.setupUi(this);
	connect(m_xmppClient, SIGNAL(contactFoundResult(int, QVariant)), this, SLOT(onContactFoundResult(int, QVariant)));
	ui.leContactJid->installEventFilter(this);

	QString server = STConfig::getConfig("/xmpp/server");
	QString port = STConfig::getConfig("/xmpp/port");
	if (server.size() == 0 || port.size() == 0)
	{
		// TODO:消息提示 没有配置server
		return;
	}
	m_xmppRegister = new XmppRegister(server, port);
	connect((const QObject *)m_xmppRegister, SIGNAL(checkResult(bool)), this, SLOT(handleCheckResult(bool)));
}

STContactAddNew::~STContactAddNew()
{

}

void STContactAddNew::initAddNewWindow()
{
	ui.leContactJid->setText("");
	ui.widSearch->setVisible(true);
	ui.widAddNewDetail->setVisible(false);
	ui.widNotFound->setVisible(false);
}

void STContactAddNew::handleCheckResult(bool result)
{
	if (!result)
	{
		//m_xmppRegister->removeAccount(m_jidName, "test");
		ui.widAddNewDetail->setVisible(false);
		ui.widNotFound->setVisible(true);
	}
	else
	{
		// 不是联系人，则查询具体信息
		m_xmppClient->queryVCard(m_jidName.append("@localhost"));
	}
}

void STContactAddNew::onContactFoundResult(int result, QVariant dataVar)
{
	if (result == 0)
	{
		ui.widAddNewDetail->setVisible(false);
		ui.widNotFound->setVisible(true);
	}
	else
	{
		UserInfo userInfo;
		userInfo = dataVar.value<UserInfo>();

		ui.lblJid->setText(userInfo.jid);
		ui.lblName->setText(userInfo.userName);

		QString path = userInfo.photoPath;
		if (path.size() == 0)
		{
			path = ":/SoftTerminal/images/account.png";
		}
		QImage* image = new QImage(path);
		ui.lblPic->setPixmap(QPixmap::fromImage(*image).scaled(80, 80));

		ui.widAddNewDetail->setVisible(true);
		ui.widNotFound->setVisible(false);

		if (result == 1)
		{
			ui.pbAddNew->setVisible(true); 
			ui.lblInfoPic->setVisible(false);
			ui.lblAlreadyFriend->setVisible(false);
			ui.lblHasSendRequest->setVisible(false);
		}
		else
		{
			ui.pbAddNew->setVisible(false);
			ui.lblInfoPic->setVisible(true);
			ui.lblAlreadyFriend->setVisible(true);
			ui.lblHasSendRequest->setVisible(false);
		}
	}
}

void STContactAddNew::on_pbSearch_clicked()
{
	printf("on_pbSearch_clicked.\n");
	QString jid = ui.leContactJid->text();
	if (jid.length() == 0)
	{
		return;
	}
	if (!jid.endsWith("@localhost"))
	{
		jid.append("@localhost");
	}
	m_jidName = jid;
	m_jidName.replace("@localhost", "");
	QList<UserInfo> friendList = m_xmppClient->getRoster();
	QList<UserInfo>::iterator it;
	for (it = friendList.begin(); it != friendList.end(); it++)
	{
		if (it->jid == jid)
		{
			// 如果已经是联系人了
			QVariant userInfoVar;
			userInfoVar.setValue(*it);
			onContactFoundResult(2, userInfoVar);
			return;
		}
	}
	// 不是联系人，则查询具体信息
	m_xmppRegister->checkAccount(m_jidName, "test");
}

void STContactAddNew::on_pbAddNew_clicked()
{
	m_xmppClient->subscribeOther(ui.lblJid->text());
	ui.pbAddNew->setVisible(false);
	ui.lblInfoPic->setVisible(true);
	ui.lblAlreadyFriend->setVisible(false);
	ui.lblHasSendRequest->setVisible(true);
}

bool STContactAddNew::eventFilter(QObject *obj, QEvent *e)
{
	if (e->type() == QEvent::KeyPress && ui.leContactJid == obj)
	{
		QKeyEvent *event = static_cast<QKeyEvent*>(e);
		if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
		{
			on_pbSearch_clicked();
			return true;
		}
	}
	return false;
}
