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
		STGroupItem(XmppGroup* group);
		~STGroupItem();
		XmppGroup* getGroup() { return m_group; };
		GroupInfo getGroupInfo() { return m_groupInfo; };
		void updateGroupPic(QString picPath);
	private:
		Ui::STGroupItemClass ui;
		XmppGroup* m_group;
		GroupInfo m_groupInfo;
	};
}
#endif
