#ifndef _STCHATDETAIL_H
#define _STCHATDETAIL_H

#include <QWidget>
#include <QTime>
#include <QKeyEvent>
#include <QToolTip>
#include <QMouseEvent>
#include "ui_STChatDetail.h"
#include "stchatrecorditem.h"
#include "stcommon.h"
#include "strecordmanager.h"
#include "xmppclient.h"
#include "stscreenshot.h"
#include "stemotion.h"
#include "ui_STScreenshotOption.h"
#include "stwhiteboard.h"
#include "stcontactitem.h"
#include "logger.h"
#include "stload.h"

using namespace tahiti;

namespace tahiti
{
	class STScreenshotOption;
	class STChatDetail : public QWidget
	{
		Q_OBJECT

	public:
		STChatDetail(XmppClient* client, STWhiteBoard* whiteboard, QWidget *parent = 0);
		~STChatDetail();
		void clearChatDetail();
		void setChatDetail(UserInfo userInfo, XmppGroup* group = NULL);
		UserInfo getUserInfo();
		void updateSelfPic(QString picPath);
		static void* loadProc(void* args);
		public Q_SLOTS:
		void on_pbSendMessage_clicked();
		void on_pbEmotion_clicked();		
		void on_pbScreenShot_clicked();
		void on_pbScreenShotOption_clicked();
		void on_pbCreateCourse_clicked();
		void on_pbJoinCourse_clicked();
		void on_pbLoadMore_clicked();
		void updateOthersMessage(RecordItem item);
		void onScreenshot();
		void onScreenshotWithoutWindow();
		void onChooseEmotion(int index);
		void onFinishScreenshot(QString path);
		void onCancelScreenshot();
		void refreshOnlineSlot();
		void onSliderChanged(int pos);
		void showMoreRecord();
		void deleteCourseSlot();
	private:
		void openScreenshot();
		void loadMoreRecord();
	Q_SIGNALS:
		void changeChatListOrder(QString jid);
		void updateOthersMessage(QString);
		void showMoreRecordSignal();
	protected:
		bool eventFilter(QObject *obj, QEvent *e);

	private:
		Ui::STChatDetailClass ui;
		UserInfo m_userInfo;
		XmppClient* m_xmppClient;
		UserInfo m_selfInfo;
		QList<STChatRecordItem*> m_recordItemList;
		STScreenshotOption* m_option;
		STEmotion* m_emotion;
		QWidget* m_main;
		STWhiteBoard* m_whiteboard;
		XmppGroup* m_group;
		QList<RecordItem> m_recordList;
		QScrollBar* m_scrollbar;
		int m_scrollNum;
		QMovie* m_movie;
		pthread_t m_tidLoad;
	};

	// ------------------------ STScreenshotOption ------------------------
	class STScreenshotOption : public QWidget
	{
		Q_OBJECT

	public:
		STScreenshotOption(QWidget* parent = 0);
		~STScreenshotOption();
	protected:
		bool eventFilter(QObject* obj, QEvent* e);
		bool event(QEvent* event);
	Q_SIGNALS:
		void screenshot();
		void screenshotWithoutWindow();
	private:
		Ui::STScreenshotOptionClass ui;

	};
}
#endif
