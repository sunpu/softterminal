#include "stwbdocwindow.h"

using namespace tahiti;

STWBDocWindow::STWBDocWindow(QWidget * parent) : QDialog(parent)
{

	ui.setupUi(this);
	//setWindowFlags(Qt::FramelessWindowHint);
	setWindowFlags(Qt::SubWindow);
}

STWBDocWindow::~STWBDocWindow()
{

}

void STWBDocWindow::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}
}

void STWBDocWindow::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint pos = this->pos() + movePoint;
		m_startMovePos = event->globalPos();
		this->move(pos.x(), pos.y());
	}
}

void STWBDocWindow::mouseReleaseEvent(QMouseEvent* event)
{
	m_isPressed = false;
}
