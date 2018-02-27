#include "stsetting.h"

using namespace tahiti;

STSetting::STSetting(QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

	QString server = STConfig::getConfig("/xmpp/server");
	ui.leServerIP->setText(server);
}

STSetting::~STSetting()
{

}

void STSetting::on_pbConfirm_clicked()
{
	STConfig::setConfig("/xmpp/server", ui.leServerIP->text());
}

void STSetting::on_pbClose_clicked()
{
	close();
}

void STSetting::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}
}

void STSetting::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint pos = this->pos() + movePoint;
		m_startMovePos = event->globalPos();
		this->move(pos.x(), pos.y());
	}
}

void STSetting::mouseReleaseEvent(QMouseEvent* event)
{
	m_isPressed = false;
}
