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

using namespace tahiti;

namespace tahiti
{
	class STScreenshotOption;
	class STChatDetail : public QWidget
	{
		Q_OBJECT

	public:
		STChatDetail(XmppClient* client, QWidget *parent = 0);
		~STChatDetail();
		void clearChatDetail();
		void setChatDetail(UserInfo userInfo);
		UserInfo getUserInfo();
		void updateSelfPic(QString picPath);
		public Q_SLOTS:
		void on_pbSendMessage_clicked();
		void on_pbEmotion_clicked();		
		void on_pbScreenShot_clicked();
		void on_pbScreenShotOption_clicked();
		void on_pbLesson_clicked();
		void updateOthersMessage(QString jid, QString msg);
		void onScreenshot();
		void onScreenshotWithoutWindow();
		void onChooseEmotion(int index);
		void onFinishScreenshot(QString path);
		void onCancelScreenshot();
	private:
		void openScreenshot();
	Q_SIGNALS:
		void changeChatListOrder(QString jid);
		void updateOthersMessage(QString);
	protected:
		bool eventFilter(QObject *obj, QEvent *e);

	private:
		Ui::STChatDetailClass ui;
		UserInfo m_userInfo;
		XmppClient* m_xmppClient;
		UserInfo m_selfInfo;
		QList<STChatRecordItem*> m_recordItemList;
		QString m_selfPicPath;
		QString m_otherPicPath;
		STScreenshotOption* m_option;
		STEmotion* m_emotion;
		QWidget* m_main;
		STWhiteBoard* m_whiteboard;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////
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
