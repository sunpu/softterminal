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
#include "stnetworkclient.h"
#include "stwbdocwindow.h"
#include "stwbvideoitem.h"
#include "logger.h"
#include "stconfig.h"
#include "token.h"
#include "woogeen/base/deviceutils.h"
#include "woogeen/base/stream.h"
#include "woogeen/conference/conferenceclient.h"
#include "woogeen/base/clientconfiguration.h"
#include "woogeen/conference/remotemixedstream.h"

using namespace std;
using namespace woogeen::conference;
using namespace woogeen::base;

namespace tahiti
{
	typedef struct StreamInfoStruct
	{
		int width = 0;
		int height = 0;
		std::shared_ptr<RemoteStream> stream = NULL;
		bool isShowing = false;
		int renderID = -1;

	} StreamInfo;

	Q_DECLARE_METATYPE(std::shared_ptr<RemoteStream>);

	class STWhiteBoard : public QWidget, public ConferenceClientObserver
	{
		Q_OBJECT
	public:
		STWhiteBoard(QWidget *parent = 0);
		~STWhiteBoard();

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
		void connectNetworkServer();
		void drawRemoteRealtimePen(QString color, int thickness, QVector<QPoint> points);
		void drawRemotePenItem(QString color, int thickness, QVector<QPoint> points, int itemID);
		void drawRemoteTextItem(QString color, int size, QString content, QPoint pos, int itemID);
		void moveRemoteItems(QPoint pos, int itemID);
		void deleteRemoteItems(QList<int> itemIDs);
		void editableAuthority(QString editable);
		void newStreamSlot(QString id, int width, int height);
		void attachRenderSlot(QString id, std::shared_ptr<RemoteStream> stream);
		void detachRenderSlot(QString id);
		void openCloudFile(QString path);
		void closeCloudFile(int index);
		void switchShowMode(bool mode);
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
	private:
		void initConferenceClient();
		void OnStreamAdded(shared_ptr<RemoteCameraStream> stream) override;
		void OnStreamAdded(shared_ptr<RemoteMixedStream> stream) override;
		void OnStreamAdded(shared_ptr<RemoteScreenStream> stream) override;
		void OnStreamRemoved(std::shared_ptr<RemoteCameraStream> stream) override;
		void OnStreamRemoved(std::shared_ptr<RemoteMixedStream> stream) override;
		void OnStreamRemoved(std::shared_ptr<RemoteScreenStream> stream) override;
		void OnUserJoined(std::shared_ptr<const User> user) override;
		void OnUserLeft(std::shared_ptr<const User> user) override;
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
	private:
		Ui::STWhiteBoardClass ui;
		STWBView* m_view;
		STWBVToolbar* m_vtoolbar;
		STWBPenStylePanel* m_penStylePanel;
		STWBTextStylePanel* m_textStylePanel;
		STNetworkClient* m_network;
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
		int m_docWindowIndex;
		int m_pen_thickness;
		QString m_pen_color;
		int m_text_size;
		QString m_text_color;
		int m_action_mode;
		int m_currentIndex;
	};
}
#endif
