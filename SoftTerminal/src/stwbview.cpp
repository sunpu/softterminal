#include "stwbview.h"
#include "stwbscene.h"

#include <QTouchEvent>
#include <QEvent>

using namespace tahiti;

STWBView::STWBView(STNetworkClient* network)
	: m_network(network)
{
	setInteractive(true);
	setOptimizationFlag(QGraphicsView::IndirectPainting);
	setCacheMode(QGraphicsView::CacheBackground);

	//antialiasing
	//setRenderHint(QPainter::HighQualityAntialiasing, true);
	setRenderHint(QPainter::Antialiasing, true);
	setObjectName("widWhiteboard");
	setStyleSheet("QWidget#widWhiteboard{background-color:#2e3037;border:0px;}");

	//init scene
	m_scene = new STWBScene(network);
	setScene(m_scene);
	setSceneRect(0, 0, 600, 600);
	resize(600, 600);
}

STWBView::~STWBView()
{

}

void STWBView::setPenColor(QString color)
{
	m_scene->setPenColor(color);
}

void STWBView::setPenThickness(int width)
{
	m_scene->setPenThickness(width);
}

void STWBView::setTextColor(QString color)
{
	m_scene->setTextColor(color);
}

void STWBView::setTextSize(int size)
{
	m_scene->setTextSize(size);
}

void STWBView::setMode(ActionType type)
{
	m_scene->setMode(type);
}

void STWBView::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event);
	setSceneRect(0, 0, width(), height());
	centerOn(width() / 2, height() / 2);
}

void STWBView::setScene(STWBScene *scene)
{
	QGraphicsView::setScene(scene);
	m_scene = scene;
}

void STWBView::deleteSelectedItem()
{
	m_scene->deleteSelectedItem();
}

void STWBView::clearSelection()
{
	m_scene->clearStatus();
}

void STWBView::drawRemoteRealtimePen(QString color, int thickness, QVector<QPoint> points)
{
	m_scene->drawRemoteRealtimePen(color, thickness, points);
}

void STWBView::drawRemotePenItem(QString color, int thickness, QVector<QPoint> points, int itemID)
{
	m_scene->drawRemotePenItem(color, thickness, points, itemID);
}

void STWBView::drawRemoteTextItem(QString color, int size, QString content, QPoint pos, int itemID)
{
	m_scene->drawRemoteTextItem(color, size, content, pos, itemID);
}

void STWBView::moveRemoteItems(QPoint pos, int itemID)
{
	m_scene->moveRemoteItems(pos, itemID);
}

void STWBView::deleteRemoteItems(QList<int> itemIDs)
{
	m_scene->deleteRemoteItems(itemIDs);
}
