#ifndef _STMAIN_H_
#define _STMAIN_H_

#include <QWidget>
#include <QMouseEvent>
#include <QList>
#include <QPainter>
#include <QDir>
#include <QtMath>
#include "ui_STMain.h"
#include "stchatdetail.h"
#include "stcontactitem.h"
#include "stcontactdetail.h"
#include "stgroupdetail.h"
#include "stcontactaddnew.h"
#include "stgroupitem.h"
#include "stgroupaddnew.h"
#include "stpersonalinfo.h"
#include "stcloudfilemanager.h"
#include "stchatitem.h"
#include "xmppclient.h"
#include "stmenu.h"
#include "stsetting.h"
#include "stmessagecenter.h"

using namespace tahiti;

namespace tahiti
{
	class XmppClient;
	class STContactAddNew;
	class STChatItem;
	class STChatDetail;
	class STContactItem;
	class STContactDetail;

	class STMain : public QWidget
	{
		Q_OBJECT

	public:
		STMain(XmppClient* client);
		~STMain();
		void init();
		void destroy();
		static void* loadProc(void* args);
		public Q_SLOTS:
		void on_pbChat_clicked();
		void on_pbContact_clicked();
		void on_pbGroup_clicked();
		void on_pbCloud_clicked();
		void on_pbAddContact_clicked();
		void on_pbMessage_clicked();
		void on_pbSetting_clicked();
		void on_pbMinimum_clicked();
		void on_pbMaximum_clicked();
		void on_pbNormal_clicked();
		void on_pbClose_clicked();
		void on_lwChatList_itemClicked();
		void on_lwContactList_itemClicked();
		void on_lwContactList_itemDoubleClicked();
		void on_lwSearchList_itemDoubleClicked();
		void on_pbNewGroup_clicked();
		void on_lwGroupList_itemClicked();
		void on_lwGroupList_itemDoubleClicked();
	Q_SIGNALS:
		void changeLoginWindow();
		void closeMain();
		void loadChatWindowSignal();
		void loadContactWindowSignal();
		void loadGroupWindowSignal();
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
		bool eventFilter(QObject* obj, QEvent* e);
	private:
		void initChatData();
		void initChatList();
		void initChatMainWindow();

		void initContactData();
		void initContactList();
		void initContactMainWindow();
		void initContactAddNew();

		void initGroupData();
		void initGroupList();
		void initGroupMainWindow();
		void initGroupAddNew();

		void refreshSearchData();
		void refreshSearchList();

		void switchChatItem(QString jid);
		void switchChatDetail(QString jid);

		void createChat(QString jid);
		void deleteChat(QString jid);

		void setPageIndex(int index);
		void loadInfo();
		private Q_SLOTS:
		void loadChatWindow();
		void loadContactWindow();
		void loadGroupWindow();
		void reorderChatList(QString jid);
		void switchChatWindow(QString jid);
		void deleteFriend(QString jid);
		void updateSelfPic(QString picPath);
		void confirmExit();
		void confirmRelogin();
		void showSettingWindow();
		void handleConfirmOK();
		void clearSearchInput();
		void onTextChanged();
		void showMessageWarn();
		void refreshContact();
		void refreshGroup(QString id);
		void updateOthersMessage(QString jid);
	private:
		Ui::STMainClass ui;
		QPoint mousePosition;
		bool isMousePressed;

		bool m_isPressed;
		QPoint m_startMovePos;
		ResizeRegion resizeRegion;
		QRect mouseDownRect;
		XmppClient* m_xmppClient;

		//STContactDetail* m_contactDetail;
		STContactAddNew* m_contactAddNew;
		STGroupAddNew* m_groupAddNew;
		//STChatDetail* m_chatDetail;

		QList<STChatItem*> m_chatItemList;
		QList<STChatDetail*> m_chatDetailList;
		QList<STContactItem*> m_contactItemList;
		STContactDetail* m_contactDetail;
		QList<STGroupItem*> m_groupItemList;
		STGroupDetail* m_groupDetail;
		QList<STContactItem*> m_searchItemList;
		STPersonalInfo* m_personalInfo;
		STCloudFileManager* m_cloudFileManager;
		STMenu* m_menu;
		STConfirm* m_confirm;
		STMessageCenter* m_messageCenterWindow;
		QString m_confirmMode;
		int m_currentPageIndex;
		QPushButton* m_clearBtn;
		QString m_currentChatJid;
		pthread_t m_tidLoad;
	};
}
#endif
