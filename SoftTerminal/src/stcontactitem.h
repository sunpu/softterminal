#ifndef _STCONTACTITEM_H_
#define _STCONTACTITEM_H_

#include <QWidget>
#include <QFile>
#include "ui_STContactItem.h"
#include "stcommon.h"

using namespace tahiti;

namespace tahiti
{
	class STContactItem : public QWidget
	{
		Q_OBJECT

	public:
		STContactItem(bool littleMode = false, bool showAddBtn = false,
			bool showDeleteBtn = false, bool showAdmin = false, QWidget* parent = 0);
		~STContactItem();
		void setUserInfo(UserInfo userInfo, bool offline = false);
		UserInfo getUserInfo();
		public Q_SLOTS:
		void on_pbAdd_clicked();
		void on_pbDelete_clicked();
	Q_SIGNALS:
		void addFriendSignal(UserInfo userInfo);
		void deleteMemberSignal(UserInfo userInfo);
	private:
		Ui::STContactItemClass ui;
		UserInfo m_userInfo;
		bool m_littleMode;
	};
}
#endif
