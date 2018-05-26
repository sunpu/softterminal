#include "stgroupdetail.h"

using namespace tahiti;

STGroupMemberItem::STGroupMemberItem(UserInfo userInfo, bool isOwner)
{
	ui.setupUi(this);

	setFixedSize(100, 80);

	ui.lblOwnerSign->setVisible(isOwner);

	if (userInfo.userName.isEmpty())
	{
		ui.lblName->setText(userInfo.jid);
	}
	else
	{
		ui.lblName->setText(userInfo.userName);
	}

	QString path = ":/SoftTerminal/images/account.png";
	if (!userInfo.photoPath.isEmpty())
	{
		path = userInfo.photoPath;
	}
	QImage* image = new QImage(path);
	ui.lblPic->setPixmap(QPixmap::fromImage(*image).scaled(60, 60));
}

STGroupMemberItem::~STGroupMemberItem()
{

}

STGroupDetail::STGroupDetail(XmppClient* client, QWidget* parent)
	: m_xmppClient(client), m_main(parent)
{
	ui.setupUi(this);

	m_confirm = new STConfirm(this);
	connect(m_confirm, SIGNAL(confirmOK()), this, SLOT(handleConfirmOK()));
}

STGroupDetail::~STGroupDetail()
{

}

void STGroupDetail::clearGroupDetail()
{
	ui.widInfo->setVisible(true);
	ui.widMain->setVisible(true);
	ui.widMessage->setVisible(false);
	ui.lblPic->clear();
	ui.lblGroupName->clear();
	ui.leGroupName->clear();
	ui.lblDesc->clear();
	ui.leDesc->clear();
	ui.lwContactList->clear();
	ui.lwGroupMemberList->clear();

	QLayoutItem *item;
	while ((item = ui.widMemberList->layout()->takeAt(0)) != 0)
	{
		if (item->widget())
		{
			delete item->widget();
		}
		delete item;
	}

	m_memberList.clear();
}

void STGroupDetail::setGroupDetail(XmppGroup* group)
{
	m_group = group;
	m_groupInfo = m_group->getGroupInfo();
	m_ownerInfo = m_group->getUserInfo(m_group->getOwner());

	clearGroupDetail();

	ui.lblGroupName->setVisible(true);
	ui.leGroupName->setVisible(false);
	ui.pbEdit->setVisible(true);
	ui.pbSave->setVisible(false);

	QString path = ":/SoftTerminal/images/group_icon.png";
	QImage* image = new QImage(path);
	ui.lblPic->setPixmap(QPixmap::fromImage(*image).scaled(80, 80));

	ui.lblGroupName->setText(m_groupInfo.name);
	ui.leGroupName->setText(m_groupInfo.name);

	if (m_groupInfo.description.isEmpty())
	{
		ui.lblDesc->setText(QStringLiteral("暂无群组说明。"));
	}
	else
	{
		ui.lblDesc->setText(m_groupInfo.description);
	}
	ui.leDesc->setText(m_groupInfo.description);
	ui.lblDesc->setVisible(true);
	ui.leDesc->setVisible(false);

	STGroupMemberItem* memberItem;
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

	QGridLayout* gridLayout = (QGridLayout*)ui.widMemberList->layout();

	memberItem = new STGroupMemberItem(m_ownerInfo, true);
	gridLayout->addWidget(memberItem, 0, 0);

	m_memberList = m_group->getMembers();

	QList<QString>::const_iterator iter;
	int i = 1;
	UserInfo info;
	for (iter = m_memberList.constBegin(); iter != m_memberList.constEnd(); iter++, i++)
	{
		info = m_group->getUserInfo(*iter);
		memberItem = new STGroupMemberItem(info);
		gridLayout->addWidget(memberItem, i / 6, i % 6);

		contactItem = new STContactItem(true, false, true);
		contactItem->setUserInfo(info);
		connect(contactItem, SIGNAL(deleteMemberSignal(UserInfo)), this, SLOT(deleteMemberSlot(UserInfo)));
		item = new QListWidgetItem();
		ui.lwGroupMemberList->addItem(item);
		ui.lwGroupMemberList->setItemWidget(item, contactItem);
	}

	ui.pbDeleteGroup->setVisible(true);
	ui.widMessage->setVisible(false);
	ui.tabWidget->setCurrentIndex(0);

	if (m_ownerInfo.jid != m_xmppClient->getSelfInfo().jid)
	{
		ui.pbEdit->setVisible(false);
		ui.tabWidget->removeTab(1);
		ui.tabWidget->removeTab(1);
	}
}

void STGroupDetail::addFriendSlot(UserInfo userInfo)
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

void STGroupDetail::deleteMemberSlot(UserInfo userInfo)
{
	int index = m_memberList.indexOf(userInfo.jid);
	ui.lwGroupMemberList->takeItem(index);
	m_memberList.removeOne(userInfo.jid);
}

void STGroupDetail::on_pbJoinChat_clicked()
{
	Q_EMIT openChatDetail(m_groupInfo.id);
}

void STGroupDetail::on_pbEdit_clicked()
{
	ui.lblGroupName->setVisible(false);
	ui.leGroupName->setVisible(true);
	ui.pbEdit->setVisible(false);
	ui.pbSave->setVisible(true);
	ui.lblDesc->setVisible(false);
	ui.leDesc->setVisible(true);
}

void STGroupDetail::on_pbSave_clicked()
{
	m_groupInfo.name = ui.leGroupName->text();
	m_groupInfo.description = ui.leDesc->text();
	m_group->setGroupInfo(m_groupInfo);

	ui.lblGroupName->setText(ui.leGroupName->text());
	ui.lblDesc->setText(ui.leDesc->text());

	ui.lblGroupName->setVisible(true);
	ui.leGroupName->setVisible(false);
	ui.pbEdit->setVisible(true);
	ui.pbSave->setVisible(false);
	ui.lblDesc->setVisible(true);
	ui.leDesc->setVisible(false);

	Q_EMIT refreshGroupSignal(m_groupInfo.id);
}

void STGroupDetail::on_pbSaveMember_clicked()
{
	m_group->setMembers(m_memberList);

	QLayoutItem *layoutItem;
	while ((layoutItem = ui.widMemberList->layout()->takeAt(0)) != 0)
	{
		if (layoutItem->widget())
		{
			delete layoutItem->widget();
		}
		delete layoutItem;
	}

	STGroupMemberItem* memberItem;
	QGridLayout* gridLayout = (QGridLayout*)ui.widMemberList->layout();

	memberItem = new STGroupMemberItem(m_ownerInfo, true);
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
	ui.tabWidget->setCurrentIndex(0);
	Q_EMIT refreshGroupSignal(m_groupInfo.id);
}

void STGroupDetail::on_pbDeleteGroup_clicked()
{
	confirmDeleteGroup();
}

void STGroupDetail::confirmDeleteGroup()
{
	m_confirm->setText(QStringLiteral("您是否确定解散该群组？"));
	int x = m_main->pos().x() + (m_main->width() - m_confirm->width()) / 2;
	int y = m_main->pos().y() + (m_main->height() - m_confirm->height()) / 2;
	m_confirm->move(QPoint(x, y));
	m_confirm->exec();
}

void STGroupDetail::handleConfirmOK()
{
	m_xmppClient->removeGroup(m_groupInfo.id);
	ui.widMain->setVisible(false);
	ui.widMessage->setVisible(true);
	Q_EMIT refreshGroupSignal("");
	Q_EMIT deleteGroupChatSignal(m_groupInfo.id);
}
