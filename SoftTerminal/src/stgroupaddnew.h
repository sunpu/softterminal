#ifndef _STGROUPADDNEW_H_
#define _STGROUPADDNEW_H_

#include <QWidget>
#include <QKeyEvent>
#include <QList>
#include <QToolTip>
#include "ui_STGroupAddNew.h"
#include "xmppclient.h"
#include "stcontactitem.h"
#include "stcommon.h"

using namespace tahiti;

namespace tahiti
{
	class STGroupAddNew : public QWidget
	{
		Q_OBJECT

	public:
		STGroupAddNew(XmppClient* client);
		~STGroupAddNew();
		void initAddNewWindow();
		public Q_SLOTS:
		void on_pbAddNew_clicked();
		void on_pbAddNewAgain_clicked();
		private Q_SLOTS:
		void addFriendSlot(UserInfo userInfo);
		void deleteMemberSlot(UserInfo userInfo);
		void createGroupResultSlot(QString id);
	Q_SIGNALS:
		void refreshGroupSignal(QString id);
	protected:
		bool eventFilter(QObject *obj, QEvent *e);

	private:
		Ui::STGroupAddNewClass ui;
		XmppClient* m_xmppClient;
		QList<QString> m_memberList;
	};
}
#endif
