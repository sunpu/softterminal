#ifndef _STSAMPLE_H
#define _STSAMPLE_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets>
#include <QMetaType>
#include "render.h"
#include "ui_STSample.h"
#include "woogeen/base/deviceutils.h"
#include "woogeen/base/stream.h"
#include "woogeen/conference/conferenceclient.h"
#include "woogeen/base/clientconfiguration.h"
#include "woogeen/conference/remotemixedstream.h"
#include "logger.h"
#include "xmppclient.h"
#include "stwbvideoitem.h"

using namespace std;
using namespace woogeen::conference;
using namespace woogeen::base;
using namespace tahiti;

namespace tahiti
{
	typedef struct StreamInfoStruct1
	{
		int width = 0;
		int height = 0;
		std::shared_ptr<RemoteStream> stream = NULL;
		bool isShowing = false;
		int renderID = -1;

	} StreamInfo1;

	//Q_DECLARE_METATYPE(std::shared_ptr<RemoteStream>);

	class STSample : public QMainWindow, public ConferenceClientObserver
	{
		Q_OBJECT
	public:
		STSample(QWidget *parent = 0);
		~STSample();

	Q_SIGNALS:
		void newStreamSignal(QString id, int width, int height);
		void updateRenderSignal(QString id, std::shared_ptr<RemoteStream> stream);

		public Q_SLOTS:
		void on_pbLogin_clicked();
		void on_pbLogout_clicked();
		void on_pbSendLocalCamera_clicked();
		void on_pbStopLocalCamera_clicked();
		void on_pbSendLocalDesktop_clicked();
		void on_pbStopLocalDesktop_clicked();
		void on_pbSendLocalWindow_clicked();
		void on_pbStopLocalWindow_clicked();

		void newStreamSlot(QString id, int width, int height);
		void updateRenderSlot(QString id, std::shared_ptr<RemoteStream> stream);
		void checkChange();

	private:
		void OnStreamAdded(shared_ptr<RemoteCameraStream> stream) override;
		void OnStreamAdded(shared_ptr<RemoteMixedStream> stream) override;
		void OnStreamAdded(shared_ptr<RemoteScreenStream> stream) override;
		void OnStreamRemoved(std::shared_ptr<RemoteCameraStream> stream) override;
		void OnStreamRemoved(std::shared_ptr<RemoteMixedStream> stream) override;
		void OnStreamRemoved(std::shared_ptr<RemoteScreenStream> stream) override;
		void OnUserJoined(std::shared_ptr<const User> user) override;
		void OnUserLeft(std::shared_ptr<const User> user) override;
		void OnServerDisconnected() override;
		void initClient();
		void subscribeStream(QString id);
		void unsubscribeStream(QString id);
		void addStream(shared_ptr<RemoteStream> stream, int width = 0, int height = 0);
		void removeStream(std::shared_ptr<RemoteStream> stream);

	private:
		Ui::STSampleClass ui;

		int m_screen_width, m_screen_height;
		bool m_hardware_accelerated;
		shared_ptr<ConferenceClient> m_client;
		//std::vector<std::shared_ptr<RemoteMixedStream>> m_mixed_stream_list;
		//QMap<QString, std::shared_ptr<RemoteStream>> m_all_stream_map;
		QMap<QString, StreamInfo1> m_all_stream_map;
		//QVector<Render *> m_renders;
		QVector<STWBVideoItem *> m_videoItems;
		QList<QCheckBox *> m_cbx_list;
		//Render* render;
		//HWND native_handle_;
		//woogeen::base::VideoRenderWindow render_window_;
		QMutex mutex;
		QString curentID;
		shared_ptr<LocalStream> m_local_camera_stream;
		shared_ptr<LocalCameraStreamParameters> m_local_camera_stream_param;
		shared_ptr<LocalStream> m_local_desktop_stream;
		shared_ptr<LocalDesktopStreamParameters> m_local_desktop_stream_param;

		std::vector< std::shared_ptr< RemoteMixedStream >> m_mixed_stream_list;

		XmppClient* m_xmpp_client;
	};
}
#endif // STSAMPLE_H
