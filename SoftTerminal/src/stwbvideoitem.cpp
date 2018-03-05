#include "stwbvideoitem.h"

using namespace tahiti;

STWBVideoItem::STWBVideoItem(QWidget * parent) : QWidget(parent)
{
	ui.setupUi(this);

	m_isusing = false;
	m_render_window.SetWindowHandle((HWND)ui.widShow->winId());
}

STWBVideoItem::~STWBVideoItem()
{

}

void STWBVideoItem::paintEvent(QPaintEvent* e)
{
	if (m_image.get())
	{
		static int frames = 0;
		frames++;
		if (!timer2.isValid())
		{
			timer2.start();
		}
		QImage qimage(m_image.get(), m_image_width, m_image_height, m_image_width * 4,
			QImage::Format_ARGB32);
		QPainter painter(this);
		painter.scale(float(this->width()) / m_image_width,
			float(this->height()) / m_image_height);
		painter.drawImage(0, 0, qimage);

		if (timer2.elapsed() >= 1000)
		{
			frames = 0;
			timer2.restart();
		}
	}
}

void STWBVideoItem::RenderFrame(std::unique_ptr<ARGBBuffer> video_frame)
{
	static int frames = 0;
	frames++;
	if (!m_timer.isValid())
	{
		m_timer.start();
	}
	if (!video_frame)
		return;
	if (m_image_width != video_frame->resolution.width ||
		m_image_height != video_frame->resolution.height)
	{
		m_image_width = video_frame->resolution.width;
		m_image_height = video_frame->resolution.height;
		m_image_size = m_image_width * m_image_height * 4;
		m_image.reset(new uint8_t[m_image_size]);
	}
	// std::copy(video_frame->buffer, video_frame->buffer + image_size_,
	// image_.get());
	memcpy(m_image.get(), video_frame->buffer, m_image_size);

	this->update();
	if (m_timer.elapsed() >= 1000)
	{
		frames = 0;
		m_timer.restart();
	}
}
