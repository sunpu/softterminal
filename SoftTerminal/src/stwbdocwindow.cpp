#include "stwbdocwindow.h"
#include "stwbscene.h"

using namespace tahiti;

STWBDocWindow::STWBDocWindow(STNetworkClient* network, int index, QWidget * parent)
	: m_network(network), m_index(index), QDialog(parent)
{

	ui.setupUi(this);
	//setWindowFlags(Qt::FramelessWindowHint);
	setWindowFlags(Qt::SubWindow);

	QVBoxLayout* layout = (QVBoxLayout*)ui.widDoc->layout();
	m_view = new STWBView(m_network, true);
	layout->addWidget(m_view);
	m_view->installEventFilter(this);

	ui.pbNormal->setVisible(false);
	ui.pbMaximum->setVisible(true);
	ui.pbMaximum->setAttribute(Qt::WA_UnderMouse, false);
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

void STWBDocWindow::on_pbMaximum_clicked()
{
	ui.pbNormal->setVisible(true);
	ui.pbNormal->setAttribute(Qt::WA_UnderMouse, false);
	ui.pbMaximum->setVisible(false);
	showFullScreen();
}

void STWBDocWindow::on_pbNormal_clicked()
{
	ui.pbNormal->setVisible(false);
	ui.pbMaximum->setVisible(true);
	ui.pbMaximum->setAttribute(Qt::WA_UnderMouse, false);
	showNormal();
}

void STWBDocWindow::on_pbClose_clicked()
{
	Q_EMIT closeCloudFile(m_index);
	close();
}

void STWBDocWindow::setActionMode(int mode)
{
	m_view->clearSelection();
	m_view->setMode((ActionType)mode);
	if (mode == ActionType::AT_Select)
	{
		m_view->setDragMode(QGraphicsView::RubberBandDrag);
	}
	else
	{
		m_view->setDragMode(QGraphicsView::NoDrag);
	}
}

void STWBDocWindow::deleteAction()
{
	m_view->deleteSelectedItem();
}

void STWBDocWindow::setPenThickness(int thickness)
{
	m_view->setPenThickness(thickness);
}

void STWBDocWindow::setPenColor(QString color)
{
	m_view->setPenColor(color);
}

void STWBDocWindow::setTextSize(int size)
{
	m_view->setTextSize(size);
}

void STWBDocWindow::setTextColor(QString color)
{
	m_view->setTextColor(color);
}
