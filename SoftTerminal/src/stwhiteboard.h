#ifndef _STWHITEBOARD_H
#define _STWHITEBOARD_H

#include <QtWidgets/QMainWindow>
#include <QMouseEvent>
#include <QGraphicsView>
#include <QListView>
#include <QMetaType>
#include <QStackedLayout>
#include "ui_STWhiteBoard.h"
#include "stwbview.h"
#include "stwbvtoolbar.h"
#include "stmessageclient.h"
#include "stwbdocwindow.h"
#include "stwbvideoitem.h"
#include "stwbcloudfileview.h"
#include "stwbinvitefriend.h"
#include "stwbroster.h"
#include "logger.h"
#include "stconfig.h"
#include "token.h"
#include "xmppclient.h"
#include "ics/base/deviceutils.h"
#include "ics/base/stream.h"
#include "ics/conference/conferenceclient.h"
#include "ics/base/clientconfiguration.h"
#include "ics/conference/remotemixedstream.h"
#include "ics/base/globalconfiguration.h"

using namespace std;
using namespace ics::conference;
using namespace ics::base;

namespace tahiti
{
	typedef struct StreamInfoStruct
	{
		int width = 0;
		int height = 0;
		std::shared_ptr<RemoteStream> stream = NULL;
		bool isShowing = false;
		int renderID = -1;
		std::shared_ptr<ConferenceSubscription> subscription;
		bool mute = false;
		QString showName;
	} StreamInfo;

	Q_DECLARE_METATYPE(std::shared_ptr<RemoteStream>);

	class ConfSubObserver : public QObject, public ics::base::SubscriptionObserver
	{
		Q_OBJECT
	public:
		ConfSubObserver(QString id);
		~ConfSubObserver();
	public:
		virtual void OnEnded();
		virtual void OnMute(TrackKind track_kind);
		virtual void OnUnmute(TrackKind track_kind);
	Q_SIGNALS:
		void subscriptionEndSignal(QString id);
	private:
		QString m_id;
	};

	class STWhiteBoard : public QWidget, public ConferenceClientObserver
	{
		Q_OBJECT
	public:
		STWhiteBoard(QString jid, QString name, XmppClient* client, QWidget *parent = 0);
		~STWhiteBoard();
		void init();
		void createCourse(QString courseID);
		void deleteCourse(QString courseID);
		QString queryCourse(QString courseID);
		void joinCourse(QString courseID);
	Q_SIGNALS:
		void newStreamSignal(QString id, int width, int height);
		void attachRenderSignal(QString id, std::shared_ptr<RemoteStream> stream);
		void detachRenderSignal(QString id);
		void setPenThicknessSignal(int thickness);
		void setPenColorSignal(QString color);
		void setTextSizeSignal(int size);
		void setTextColorSignal(QString color);
		void setActionModeSignal(int mode);
		void deleteActionSignal();
		void showLocalCameraSignal();
		void unshowLocalCameraSignal();
		void muteResultSignal(bool result, QString);
		void unmuteResultSignal(bool result, QString);
		void deleteCourseSignal();
		private Q_SLOTS:
		void setPenThickness(int thickness);
		void setPenColor(QString color);
		void setTextSize(int size);
		void setTextColor(QString color);
		void setActionMode(int mode);
		void deleteAction();
		void hideStylePanels();
		void showPenStylePanel();
		void showTextStylePanel();
		void whiteBoardMessageSlot(QString message);
		void newStreamSlot(QString id, int width, int height);
		void attachRenderSlot(QString id, std::shared_ptr<RemoteStream> stream);
		void detachRenderSlot(QString id);
		void openCloudFile(QString path);
		void closeCloudFile(int index);
		void switchShowMode(bool mode);
		void showLocalCamera();
		void unshowLocalCamera();
		void mute(QString id);
		void unmute(QString id);
		void subscriptionEnd(QString id);
		void openCloudFileView();
		void closeCloudFileView();
		void openInviteFriend();
		void closeInviteFriend();
		void openRoster();
		void closeRoster();
		void deleteCourse();
		public Q_SLOTS:
		void on_pbMinimum_clicked();
		void on_pbMaximum_clicked();
		void on_pbNormal_clicked();
		void on_pbClose_clicked();
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
		bool eventFilter(QObject* watched, QEvent* e);
		void resizeEvent(QResizeEvent* size);
		void showEvent(QShowEvent* e);
	private:
		void initConferenceClient();
		void OnStreamAdded(std::shared_ptr<ics::conference::RemoteStream> stream);
		void OnStreamRemoved(std::shared_ptr<ics::conference::RemoteStream> stream);
		void OnServerDisconnected() override;
		void subscribeStream(QString id);
		void unsubscribeStream(QString id);
		void addStream(shared_ptr<RemoteStream> stream, int width = 0, int height = 0);
		void removeStream(std::shared_ptr<RemoteStream> stream);
		void login();
		void logout();
		void sendLocalCamera();
		void stopSendLocalCamera();
		void resizeMaximumDocWindow();
		void setClientAuthority(QString editable);
		void drawRemoteRealtimePen(QString color, int thickness, QVector<QPoint> points);
		void drawRemotePenItem(QString color, int thickness, QVector<QPoint> points, QString itemID);
		void drawRemoteTextItem(QString color, int size, QString content, QPoint pos, QString itemID);
		void moveRemoteItems(QPoint pos, QString itemID);
		void deleteRemoteItems(QList<QString> itemIDs);
		void editableAuthority(QString editable);
	private:
		Ui::STWhiteBoardClass ui;
		STWBView* m_view;
		STWBRaiseHandPanel* m_raiseHandPanel;
		STWBVToolbar* m_vtoolbar;
		STWBPenStylePanel* m_penStylePanel;
		STWBTextStylePanel* m_textStylePanel;
		STMessageClient* m_messageClient;
		bool m_isPressed;
		QPoint m_startMovePos;
		int m_screen_width, m_screen_height;
		bool m_hardware_accelerated;
		shared_ptr<ConferenceClient> m_client;
		QVector<STWBVideoItem*> m_videoItems;
		QVector<STWBVideoItem*> m_big_videoItems;
		QMap<QString, StreamInfo> m_all_stream_map;
		QString m_curentID;
		shared_ptr<LocalStream> m_local_camera_stream;
		shared_ptr<LocalCameraStreamParameters> m_local_camera_stream_param;
		QVector<STWBDocWindow*> m_docWindows;
		STWBCloudFileView* m_cloud_file_view;
		STWBInviteFriend* m_inviteFriend;
		STWBRoster* m_roster;
		int m_docWindowIndex;
		int m_pen_thickness;
		QString m_pen_color;
		int m_text_size;
		QString m_text_color;
		int m_action_mode;
		int m_currentIndex;
		int m_localCameraRenderID;
		mutable std::mutex m_callback_mutex;
		QString m_jid;
		QString m_name;
		std::shared_ptr<ConferencePublication> m_publication;
		XmppClient* m_xmppClient;
		QString m_courseID;
	};
}
#endif
