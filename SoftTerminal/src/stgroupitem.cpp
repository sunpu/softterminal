#include "stgroupitem.h"

using namespace tahiti;

STGroupItem::STGroupItem(XmppClient* client, XmppGroup* group)
	: m_xmppClient(client), m_group(group)
{
	ui.setupUi(this);
	connect(m_xmppClient, SIGNAL(contactFoundResult(int, QVariant)), this, SLOT(onContactFoundResult(int, QVariant)));

	m_groupInfo = m_group->getGroupInfo();

	ui.lblGroupOwner->setText(m_group->getOwner());
	ui.lblGroupName->setText(m_groupInfo.name);
	QImage* image = new QImage(":/SoftTerminal/images/group_icon.png");
	ui.lblGroupPic->setPixmap(QPixmap::fromImage(*image).scaled(36, 36));

	m_xmppClient->queryVCard(m_group->getOwner());
	QList<QString> members = m_group->getMembers();
	QList<QString>::iterator it;
	for (it = members.begin(); it != members.end(); it++)
	{
		m_xmppClient->queryVCard(*it);
	}
}

STGroupItem::~STGroupItem()
{

}

void STGroupItem::onContactFoundResult(int result, QVariant dataVar)
{
	if (result == 1)
	{
		UserInfo userInfo;
		userInfo = dataVar.value<UserInfo>();
		QString jid = userInfo.jid;

		if (jid == m_group->getOwner())
		{
			ui.lblGroupOwner->setText(userInfo.userName);
			m_ownerInfo = userInfo;
		}
		else if (m_group->getMembers().contains(jid))
		{
			m_membersInfoMap.insert(userInfo.jid, userInfo);
		}
	}
}
