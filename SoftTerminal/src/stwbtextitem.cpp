#include "stwbtextitem.h"
#include <QUuid>
#include <QBrush>
#include <QPen>

using namespace tahiti;

STWBTextItem::STWBTextItem(QString itemID)
	: m_itemID(itemID)
{
	setFlags(QGraphicsItem::ItemIsMovable |
		QGraphicsItem::ItemIsFocusable |
		QGraphicsItem::ItemIsSelectable);
	setTextInteractionFlags(Qt::TextEditorInteraction);
}

void STWBTextItem::focusOutEvent(QFocusEvent* event)
{
	setTextInteractionFlags(Qt::NoTextInteraction);
	QString content = this->toPlainText();
	if (content.size() == 0)
	{
		return;
	}
	QPoint pos = this->pos().toPoint();
	((STWBScene*)scene())->drawLocalTextItem(content, pos, m_itemID);
}

void STWBTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	setTextInteractionFlags(Qt::TextEditorInteraction);
	setFocus();
}
