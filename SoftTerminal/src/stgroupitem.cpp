#include "stgroupitem.h"

using namespace tahiti;

STGroupItem::STGroupItem(XmppGroup* group) : m_group(group)
{
	ui.setupUi(this);

	m_groupInfo = m_group->getGroupInfo();

	ui.lblGroupOwner->setText(m_group->getUserInfo(m_group->getOwner()).userName);
	ui.lblGroupName->setText(m_groupInfo.name);

	QString avatarPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
		+ DATA_ROOT_PATH + AVATAR_PATH;
	QString avatarFile = avatarPath + m_groupInfo.id + ".png";
	if (!QFile::exists(avatarFile))
	{
		avatarFile = ":/SoftTerminal/images/group_icon.png";
	}

	QImage* image = new QImage(avatarFile);
	ui.lblGroupPic->setPixmap(QPixmap::fromImage(*image).scaled(36, 36));
}

STGroupItem::~STGroupItem()
{

}

void STGroupItem::updateGroupPic(QString picPath)
{
	QImage* image = new QImage(picPath);
	ui.lblGroupPic->setPixmap(QPixmap::fromImage(*image).scaled(36, 36));
}
