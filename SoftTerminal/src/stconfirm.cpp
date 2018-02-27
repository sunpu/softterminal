#include "stconfirm.h"

using namespace tahiti;

STConfirm::STConfirm(QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);
	//setWindowModality(Qt::ApplicationModal);
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
}

STConfirm::~STConfirm()
{

}

void STConfirm::setText(QString text)
{
	ui.lblText->setText(text);
}

void STConfirm::on_pbOK_clicked()
{
	Q_EMIT confirmOK();
	close();
}

void STConfirm::on_pbCancel_clicked()
{
	close();
}

void STConfirm::on_pbClose_clicked()
{
	close();
}

void STConfirm::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}
}

void STConfirm::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint pos = this->pos() + movePoint;
		m_startMovePos = event->globalPos();
		this->move(pos.x(), pos.y());
	}
}

void STConfirm::mouseReleaseEvent(QMouseEvent* event)
{
	m_isPressed = false;
}
