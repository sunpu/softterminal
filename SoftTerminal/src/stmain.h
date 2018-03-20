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
#include "stcontactaddnew.h"
#include "stpersonalinfo.h"
#include "stcloudfilemanager.h"
#include "stchatitem.h"
#include "xmppclient.h"
#include "stmenu.h"
#include "stsetting.h"

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
	Q_SIGNALS:
		void changeLoginWindow();
		void closeMain();
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
		bool eventFilter(QObject* obj, QEvent* e);
	private:
		void initChatWindow();
		void initChatData();
		void initChatList();
		void initChatMainWindow();

		void initContactWindow();
		void initContactData();
		void initContactList();
		void initContactMainWindow();

		void initContactAddNew();

		void refreshSearchData();
		void refreshSearchList();

		void switchChatItem(QString jid);
		void switchChatDetail(QString jid);

		void newChat(QString jid);
		void deleteChat(QString jid);

		void setPageIndex(int index);

		private Q_SLOTS:
		void reorderChatList(QString jid);
		void switchChatWindow(QString jid);
		void updateSelfPic(QString picPath);
		void confirmExit();
		void confirmRelogin();
		void showSettingWindow();
		void handleConfirmOK();
		void clearSearchInput();
		void onTextChanged();
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
		//STChatDetail* m_chatDetail;

		QList<STChatItem*> m_chatItemList;
		QList<STChatDetail*> m_chatDetailList;
		QList<STContactItem*> m_contactItemList;
		QList<STContactItem*> m_searchItemList;
		STContactDetail* m_contactDetail;
		STPersonalInfo* m_personalInfo;
		STCloudFileManager* m_cloudFileManager;
		STMenu* m_menu;
		STConfirm* m_confirm;
		QString m_confirmMode;
		int m_currentPageIndex;
		QPushButton* m_clearBtn;
	};
}
#endif
