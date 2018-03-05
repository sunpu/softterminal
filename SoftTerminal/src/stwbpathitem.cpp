#include "stwbpathitem.h"
#include <QUuid>
#include <QBrush>
#include <QPen>

using namespace tahiti;

STWBPathItem::STWBPathItem(int itemID)
	:m_itemID(itemID)
{
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
}

void STWBPathItem::setColor(QString color)
{
	m_color = color;
}

void STWBPathItem::setThickness(int thickcness)
{
	m_thickness = thickcness;
}

void STWBPathItem::render()
{
	if (m_listPoints.size() < 1)
	{
		return;
	}

	QPainterPath path;
	path.moveTo(m_listPoints[0]);
	for (int i = 1; i < m_listPoints.size(); i++)
	{
		path.lineTo(m_listPoints[i]);
	}

	this->setPen(QPen(QBrush(QColor(m_color)), (qreal)m_thickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	this->setPath(path);
}
