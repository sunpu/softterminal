#ifndef STWBPATHITEM_H
#define STWBPATHITEM_H

#include <QGraphicsPathItem>
#include <vector>
#include <QPointF>
#include "stcommon.h"

namespace tahiti
{
	class STWBPathItem : public QGraphicsPathItem
	{
	public:
		STWBPathItem(int itemID);

		void setColor(QString color);
		void setThickness(int thickcness);

		QString color() { return m_color; }
		int thickness() { return m_thickness; }
		int itemID() { return m_itemID; }

		void render(void);
		QVector<QPoint> points(void) { return m_listPoints; }
		void addPoint(const QPoint& pt)
		{
			m_listPoints.push_back(pt);
		}
	private:
		QVector<QPoint> m_listPoints;
		QString m_color;
		int m_thickness;
		ActionType m_type;
		int m_itemID;
	};
}
#endif
