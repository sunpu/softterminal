#ifndef _STWBTEXTITEM_H
#define _STWBTEXTITEM_H

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
		STWBTextItem(QString itemID);
		QString itemID() { return m_itemID; }
	protected:
		void focusOutEvent(QFocusEvent* event);
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
	private:
		ActionType m_type;
		QString m_itemID;
	};
}
#endif
