#include "stwbvideoitem.h"

using namespace tahiti;

STWBVideoItem::STWBVideoItem(QWidget * parent) : QWidget(parent)
{
	ui.setupUi(this);

	unuse();
	m_render_window.SetWindowHandle((HWND)ui.widShow->winId());
}

STWBVideoItem::~STWBVideoItem()
{

}

void STWBVideoItem::setRenderSize(int width, int height)
{
	setMaximumSize(width, height + 25);
	setMinimumSize(width, height + 25);
	ui.widShow->setMaximumSize(width, height);
	ui.widShow->setMinimumSize(width, height);
	ui.widToolbar->setMaximumSize(width, 25);
	ui.widToolbar->setMinimumSize(width, 25);
}

void STWBVideoItem::setBgImage(QString url)
{
	ui.widShow->setStyleSheet(QString("border-image: url(%1);"
		"border-top-left-radius: 3px;"
		"border-top-right-radius: 3px;").arg(url));
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

void STWBVideoItem::use(QString id, QString showName, bool mute)
{
	m_isusing = true;
	m_id = id;
	ui.lblName->setText(showName);

	if (id.isEmpty())
	{
		ui.pbMute->setVisible(false);
		ui.pbUnmute->setVisible(false);
	}
	else
	{
		ui.pbMute->setVisible(!mute);
		ui.pbUnmute->setVisible(mute);
	}
}

void STWBVideoItem::unuse()
{
	m_isusing = false;
	ui.lblName->clear();
	ui.pbMute->setVisible(true);
	ui.pbUnmute->setVisible(false);
	m_id.clear();
}

void STWBVideoItem::on_pbMute_clicked()
{
	Q_EMIT muteSignal(m_id);
}

void STWBVideoItem::on_pbUnmute_clicked()
{
	Q_EMIT unmuteSignal(m_id);
}

void STWBVideoItem::onMuteResult(bool result, QString id)
{
	if (m_id == id)
	{
		ui.pbMute->setVisible(!result);
		ui.pbUnmute->setVisible(result);
	}
}

void STWBVideoItem::onUnmuteResult(bool result, QString id)
{
	if (m_id == id)
	{
		ui.pbMute->setVisible(result);
		ui.pbUnmute->setVisible(!result);
	}
}
