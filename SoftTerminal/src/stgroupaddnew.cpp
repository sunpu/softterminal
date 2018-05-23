#include "stgroupaddnew.h"

using namespace tahiti;

STGroupAddNew::STGroupAddNew(XmppClient* client) : m_xmppClient(client)
{
	ui.setupUi(this);

	qRegisterMetaType<UserInfo>("UserInfo");
	connect(m_xmppClient, SIGNAL(createGroupResultSignal(QString)), this, SLOT(createGroupResultSlot(QString)));
}

STGroupAddNew::~STGroupAddNew()
{

}

void STGroupAddNew::initAddNewWindow()
{
	ui.lblInfoPic->setVisible(false);
	ui.lblHasAddNew->setVisible(false);
	ui.pbAddNewAgain->setVisible(false);
	ui.pbAddNew->setVisible(true);
	ui.leGroupName->clear();
	ui.leGroupDesc->clear();
	ui.leGroupName->setVisible(true);
	ui.leGroupDesc->setVisible(true);
	ui.lblGroupName->setVisible(false);
	ui.lblGroupDesc->setVisible(false);
	ui.widMemberPreview->setVisible(false);
	ui.widMember->setVisible(true);
	ui.lwContactList->clear();
	ui.lwGroupMemberList->clear();
	m_memberList.clear();

	QLayoutItem *layoutItem;
	while ((layoutItem = ui.widMemberList->layout()->takeAt(0)) != 0)
	{
		if (layoutItem->widget())
		{
			delete layoutItem->widget();
		}
		delete layoutItem;
	}

	STContactItem* contactItem;
	QListWidgetItem* item;

	QList<UserInfo> friendList = m_xmppClient->getRoster();
	QList<UserInfo>::const_iterator it;
	for (it = friendList.constBegin(); it != friendList.constEnd(); it++)
	{
		contactItem = new STContactItem(true, true, false);
		contactItem->setUserInfo(*it);
		connect(contactItem, SIGNAL(addFriendSignal(UserInfo)), this, SLOT(addFriendSlot(UserInfo)));
		item = new QListWidgetItem();
		ui.lwContactList->addItem(item);
		ui.lwContactList->setItemWidget(item, contactItem);
	}
}

void STGroupAddNew::addFriendSlot(UserInfo userInfo)
{
	if (m_memberList.contains(userInfo.jid))
	{
		return;
	}
	STContactItem* contactItem = new STContactItem(true, false, true);
	connect(contactItem, SIGNAL(deleteMemberSignal(UserInfo)), this, SLOT(deleteMemberSlot(UserInfo)));
	contactItem->setUserInfo(userInfo);
	QListWidgetItem* item = new QListWidgetItem();
	ui.lwGroupMemberList->addItem(item);
	ui.lwGroupMemberList->setItemWidget(item, contactItem);
	m_memberList.append(userInfo.jid);
}

void STGroupAddNew::deleteMemberSlot(UserInfo userInfo)
{
	int index = m_memberList.indexOf(userInfo.jid);
	ui.lwGroupMemberList->takeItem(index);
	m_memberList.removeOne(userInfo.jid);
}

void STGroupAddNew::on_pbAddNew_clicked()
{
	GroupInfo info;
	info.name = ui.leGroupName->text();
	info.description = ui.leGroupDesc->text();
	m_xmppClient->createGroup(info, m_memberList);
}

void STGroupAddNew::on_pbAddNewAgain_clicked()
{
	initAddNewWindow();
}

void STGroupAddNew::createGroupResultSlot(QString id)
{
	if (!id.isEmpty())
	{
		STGroupMemberItem* memberItem;
		QGridLayout* gridLayout = (QGridLayout*)ui.widMemberList->layout();

		memberItem = new STGroupMemberItem(m_xmppClient->getSelfInfo(), true);
		gridLayout->addWidget(memberItem, 0, 0);

		QList<UserInfo> friendList = m_xmppClient->getRoster();
		QList<UserInfo>::const_iterator it;
		QList<QString>::const_iterator iter;
		int  i = 1;
		STContactItem* contactItem;
		QListWidgetItem* item;

		for (iter = m_memberList.constBegin(); iter != m_memberList.constEnd(); iter++, i++)
		{
			for (it = friendList.constBegin(); it != friendList.constEnd(); it++)
			{
				if (*iter == it->jid)
				{
					memberItem = new STGroupMemberItem(*it);
					gridLayout->addWidget(memberItem, i / 6, i % 6);
					break;
				}
			}
		}

		ui.lblInfoPic->setVisible(true);
		ui.lblHasAddNew->setVisible(true);
		ui.pbAddNewAgain->setVisible(true);
		ui.pbAddNew->setVisible(false);
		ui.leGroupName->setVisible(false);
		ui.leGroupDesc->setVisible(false);
		ui.lblGroupName->setText(ui.leGroupName->text());
		ui.lblGroupDesc->setText(ui.leGroupDesc->text());
		ui.widMemberPreview->setVisible(true);
		ui.widMember->setVisible(false);
		ui.lblGroupName->setVisible(true);
		ui.lblGroupDesc->setVisible(true);
		Q_EMIT refreshGroupSignal("");
	}
	else
	{
		ui.leGroupName->setFocus();
		QPoint pos(ui.leGroupName->mapToGlobal(ui.leGroupName->pos()).x() - ui.leGroupName->width() / 2,
			ui.leGroupName->mapToGlobal(ui.leGroupName->pos()).y());
		QToolTip::showText(pos, QStringLiteral("创建群组失败"), ui.leGroupName);
	}
}

bool STGroupAddNew::eventFilter(QObject *obj, QEvent *e)
{
	return false;
}
