#ifndef _STGROUPITEM_H_
#define _STGROUPITEM_H_

#include <QWidget>
#include "ui_STGroupItem.h"
#include "stcommon.h"
#include "xmppclient.h"

using namespace tahiti;

namespace tahiti
{
	class STGroupItem : public QWidget
	{
		Q_OBJECT

	public:
		STGroupItem(XmppClient* client, XmppGroup* group);
		~STGroupItem();
		XmppGroup* getGroup() { return m_group; };
		GroupInfo getGroupInfo() { return m_groupInfo; };
		UserInfo getOwnerInfo() { return m_ownerInfo; };
		QMap<QString, UserInfo> getMembersInfo() { return m_membersInfoMap; };
		private Q_SLOTS:
		void onContactFoundResult(int result, QVariant dataVar);
	private:
		Ui::STGroupItemClass ui;
		XmppClient* m_xmppClient;
		XmppGroup* m_group;
		GroupInfo m_groupInfo;
		UserInfo m_ownerInfo;
		QMap<QString, UserInfo> m_membersInfoMap;
	};
}
#endif
