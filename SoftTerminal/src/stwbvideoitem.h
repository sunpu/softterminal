#ifndef _STWBVIDEOITEM_H
#define _STWBVIDEOITEM_H

#include <QWidget>
#include "ui_STWBVideoItem.h"
#include <QtEvents>
#include <QtWidgets>
#include <fstream>
#include <memory>
#include <mutex>
#include "ics/base/stream.h"
#include "ics/base/videorendererinterface.h"

using namespace std;
using namespace ics::base;

namespace tahiti
{
	class STWBVideoItem : public QWidget, public VideoRendererARGBInterface
	{
		Q_OBJECT

	public:
		STWBVideoItem(QWidget * parent = 0);
		~STWBVideoItem();
		void setRenderSize(int width, int height);
		void setBgImage(QString url);
		bool isusing() { return m_isusing; };
		void use(QString id, QString showName, bool mute);
		void unuse();
		ics::base::VideoRenderWindow getRenderWindow() { return m_render_window; };
		public Q_SLOTS:
		void on_pbMute_clicked();
		void on_pbUnmute_clicked();
	Q_SIGNALS:
		void muteSignal(QString id);
		void unmuteSignal(QString id);
		private Q_SLOTS:
		void onMuteResult(bool result);
		void onUnmuteResult(bool result);
	private:
		void paintEvent(QPaintEvent* e);
		void RenderFrame(std::unique_ptr<ARGBBuffer> video_frame);
	private:
		Ui::STWBVideoItemClass ui;
		QTime m_timer, timer2;
		unique_ptr<uint8_t[]> m_image;
		int m_image_width, m_image_height, m_image_size;
		bool m_isusing;
		ics::base::VideoRenderWindow m_render_window;
		QString m_id;
	};
}
#endif
