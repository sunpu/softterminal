#ifndef _STWBVIDEOITEM_H
#define _STWBVIDEOITEM_H

#include <QWidget>
#include "ui_STWBVideoItem.h"
#include <QtEvents>
#include <QtWidgets>
#include <fstream>
#include <memory>
#include <mutex>
#include "woogeen/base/stream.h"
#include "woogeen/base/videorendererinterface.h"

using namespace std;
using namespace woogeen::base;

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
		bool isusing() { return m_isusing; }
		void use() { m_isusing = true; }
		void unuse() { m_isusing = false; }
		woogeen::base::VideoRenderWindow getRenderWindow() { return m_render_window; }
	private:
		void paintEvent(QPaintEvent* e);
		void RenderFrame(std::unique_ptr<ARGBBuffer> video_frame);
	private:
		Ui::STWBVideoItemClass ui;
		QTime m_timer, timer2;
		unique_ptr<uint8_t[]> m_image;
		int m_image_width, m_image_height, m_image_size;
		bool m_isusing;
		woogeen::base::VideoRenderWindow m_render_window;
	};
}
#endif
