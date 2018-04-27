#ifndef _RENDER_H
#define _RENDER_H

#include <QtEvents>
#include <QtWidgets/QMainWindow>
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
	class Render : public QWidget, public VideoRendererARGBInterface
	{
		Q_OBJECT
	public:
		Render(QWidget* parent = 0, Qt::WindowFlags f = 0);
		~Render();
		bool isusing() { return m_isusing; }
		void use() { m_isusing = true; }
		void unuse() { m_isusing = false; }
		ics::base::VideoRenderWindow getRenderWindow() { return render_window_; }
	private:
		QTime timer, timer2;
		void paintEvent(QPaintEvent* e);
		unique_ptr<uint8_t[]> image_;
		int image_width_, image_height_, image_size_;
		void RenderFrame(std::unique_ptr<ARGBBuffer> video_frame);
		// void SetSize(int width, int height);
		bool m_isusing;
		ics::base::VideoRenderWindow render_window_;
	};
}
#endif
