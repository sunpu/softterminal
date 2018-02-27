#include <iostream>
#include "render.h"

using namespace tahiti;

Render::Render(QWidget* parent, Qt::WindowFlags f)
{
	this->setParent(parent);
	this->setWindowFlags(f);

	this->setMinimumSize(QSize(320, 180));
	this->setMaximumSize(QSize(320, 180));

	m_isusing = false;

	render_window_.SetWindowHandle((HWND)this->winId());
}

Render::~Render() {}

/*void Render::paintEvent(QPaintEvent* e)
{
	if (image_.get())
	{
		static int frames = 0;
		frames++;
		if (!timer2.isValid())
		{
			timer2.start();
		}
		QImage qimage(image_.get(), image_width_, image_height_, image_width_ * 4,
			QImage::Format_ARGB32);
		QPainter painter(this);
		painter.scale(float(this->width()) / image_width_,
			float(this->height()) / image_height_);
		painter.drawImage(0, 0, qimage);

		if (timer2.elapsed() >= 1000)
		{
			std::cout << "rps: " << frames << std::endl;
			frames = 0;
			timer2.restart();
		}
	}
}*/

void Render::RenderFrame(std::unique_ptr<ARGBBuffer> video_frame)
{
	return;
	static int frames = 0;
	frames++;
	if (!timer.isValid())
	{
		timer.start();
	}
	if (!video_frame)
		return;
	if (image_width_ != video_frame->resolution.width ||
		image_height_ != video_frame->resolution.height)
	{
		image_width_ = video_frame->resolution.width;
		image_height_ = video_frame->resolution.height;
		image_size_ = image_width_ * image_height_ * 4;
		image_.reset(new uint8_t[image_size_]);
	}
	// std::copy(video_frame->buffer, video_frame->buffer + image_size_,
	// image_.get());
	memcpy(image_.get(), video_frame->buffer, image_size_);

	this->update();
	if (timer.elapsed() >= 1000)
	{
		std::cout << "fps: " << frames << std::endl;
		frames = 0;
		timer.restart();
	}
}
