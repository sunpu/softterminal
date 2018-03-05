#ifndef STWBTEXTITEM_H
#define STWBTEXTITEM_H

#include <QGraphicsTextItem>
#include <vector>
#include <QPointF>
#include <QFont>
#include "stcommon.h"
#include "stwbscene.h"

namespace tahiti
{
	class STWBScene;
	class STWBTextItem : public QGraphicsTextItem
	{
	public:
		STWBTextItem(int itemID);
		int itemID() { return m_itemID; }
	protected:
		void focusOutEvent(QFocusEvent* event);
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
	private:
		ActionType m_type;
		int m_itemID;
	};
}
#endif
