#include "stgroupitem.h"

using namespace tahiti;

STGroupItem::STGroupItem(XmppGroup* group) : m_group(group)
{
	ui.setupUi(this);

	m_groupInfo = m_group->getGroupInfo();

	ui.lblGroupOwner->setText(m_group->getUserInfo(m_group->getOwner()).userName);
	ui.lblGroupName->setText(m_groupInfo.name);
	QImage* image = new QImage(":/SoftTerminal/images/group_icon.png");
	ui.lblGroupPic->setPixmap(QPixmap::fromImage(*image).scaled(36, 36));
}

STGroupItem::~STGroupItem()
{

}
