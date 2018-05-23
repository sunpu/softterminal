﻿#ifndef _STGROUPDETAIL_H_
#define _STGROUPDETAIL_H_

#include <QWidget>
#include <QScrollArea>
#include "ui_STGroupDetail.h"
#include "ui_STGroupMemberItem.h"
#include "stcommon.h"
#include "stcontactitem.h"
#include "xmppclient.h"

using namespace tahiti;

namespace tahiti
{
	class STGroupMemberItem : public QWidget
	{
		Q_OBJECT

	public:
		STGroupMemberItem(UserInfo userInfo, bool isOwner = false);
		~STGroupMemberItem();
	private:
		Ui::STGroupMemberItemClass ui;
	};

	class STGroupDetail : public QWidget
	{
		Q_OBJECT

	public:
		STGroupDetail(XmppClient* client);
		~STGroupDetail();
		void clearGroupDetail();
		void setGroupDetail(XmppGroup* group, GroupInfo groupInfo, UserInfo ownerInfo, QMap<QString, UserInfo> membersInfo);
		public Q_SLOTS:
		void on_pbJoinChat_clicked();
		void on_pbEdit_clicked();
		void on_pbSave_clicked();
		void on_pbSaveMember_clicked();
		void on_pbDeleteGroup_clicked();
		private Q_SLOTS:
		void addFriendSlot(UserInfo userInfo);
		void deleteMemberSlot(UserInfo userInfo);
	Q_SIGNALS:
		void refreshGroupSignal(QString id);
	private:
		Ui::STGroupDetailClass ui;
		XmppClient* m_xmppClient;
		XmppGroup* m_group;
		GroupInfo m_groupInfo;
		UserInfo m_ownerInfo;
		QList<QString> m_memberList;
	};
}
#endif
