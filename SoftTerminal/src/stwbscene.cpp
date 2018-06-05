#include "stwbscene.h"

using namespace tahiti;

STWBScene::STWBScene()
{
	m_isDrawing = false;

	m_realtimePathItemData = NULL;
	m_pathItemData = NULL;
	m_textItem = NULL;
	m_last_textItem = NULL;

	m_messageClient = new STMessageClient;
}

void STWBScene::setCourseID(QString courseID)
{
	m_courseID = courseID;
}

void STWBScene::setMode(ActionType type)
{
	m_type = type;
}

void STWBScene::setPenColor(QString color)
{
	m_pen_color = color;
}

void STWBScene::setPenThickness(int w)
{
	m_pen_thickness = w;
}

void STWBScene::setTextColor(QString color)
{
	m_text_color = color;
}

void STWBScene::setTextSize(int size)
{
	m_text_size = size;
}

void STWBScene::drawStart(PathItemData *dt)
{
	double x = dt->prePoint.x() - (double(m_pen_thickness) / 2.f);
	double y = dt->prePoint.y() - (double(m_pen_thickness) / 2.f);

	QGraphicsEllipseItem* el = addEllipse(QRect(x, y, m_pen_thickness, m_pen_thickness),
		QPen(QColor(m_pen_color), 1), QBrush(QColor(m_pen_color)));

	dt->tempDrawingItem.push_back(el);
}

void STWBScene::drawTo(PathItemData *dt, const QPoint &to)
{
	QGraphicsLineItem* li = addLine(dt->prePoint.x(), dt->prePoint.y(), to.x(), to.y(),
		QPen(QBrush(QColor(m_pen_color)), m_pen_thickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

	dt->tempDrawingItem.push_back(li);
}

void STWBScene::onPenDown(QPoint pt, int id)
{
	QString uuid = QUuid::createUuid().toString().remove("{").remove("}").remove("-");
	m_pathItemData = new PathItemData;
	m_pathItemData->prePoint = pt;//scene point;
	m_pathItemData->pathItem = new STWBPathItem(uuid);
	m_pathItemData->pathItem->addPoint(pt);

	drawStart(m_pathItemData);
}

void STWBScene::onPenMove(QPoint pt, int id)
{
	QPoint to = pt;

	if (m_pathItemData->pathItem)
	{
		m_pathItemData->pathItem->addPoint(to);
	}

	drawTo(m_pathItemData, to);
	m_pathItemData->prePoint = to;
}

void STWBScene::onPenUp(int id)
{
	for (int i = 0; i < m_pathItemData->tempDrawingItem.size(); i++)
	{
		QGraphicsScene::removeItem(m_pathItemData->tempDrawingItem[i]);
	}

	m_pathItemData->tempDrawingItem.clear();
	m_pathItemData->pathItem->setColor(m_pen_color);
	m_pathItemData->pathItem->setThickness(m_pen_thickness);
	m_pathItemData->pathItem->render();
	QGraphicsScene::addItem(m_pathItemData->pathItem);
	m_itemMap[m_pathItemData->pathItem->itemID()] = m_pathItemData->pathItem;

	drawPenItem(m_pen_color, m_pen_thickness,
		m_pathItemData->pathItem->points(), m_pathItemData->pathItem->itemID());
}

void STWBScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_type == ActionType::AT_Pen)
	{
		m_isDrawing = true;
		onPenDown(event->scenePos().toPoint());
		m_realtimePoints.clear();
		m_realtimePoints.append(event->scenePos().toPoint());
	}
	else if (m_type == ActionType::AT_Text)
	{
		if (m_textItem != NULL)
		{
			m_last_textItem = m_textItem;
			m_last_textItem->setTextInteractionFlags(Qt::NoTextInteraction);
			m_last_textItem->setSelected(false);
			if (m_last_textItem->toPlainText().length() == 0)
			{
				QGraphicsScene::removeItem(m_last_textItem);
				m_last_textItem = NULL;
			}
		}
		QString uuid = QUuid::createUuid().toString().remove("{").remove("}").remove("-");
		m_textItem = new STWBTextItem(uuid);
		m_textItem->setDefaultTextColor(m_text_color);
		m_textItem->setPlainText(QString::fromLocal8Bit(""));
		QFont font = m_textItem->font();
		font.setPixelSize(m_text_size);
		m_textItem->setFont(font);
		m_textItem->setSelected(true);
		m_textItem->setFocus();
		m_textItem->setPos(event->scenePos().toPoint());
		QGraphicsScene::addItem(m_textItem);
		m_itemMap[m_textItem->itemID()] = m_textItem;
	}
	else if (m_type == ActionType::AT_Select)
	{
		QGraphicsScene::mousePressEvent(event);
	}
	else
	{
		return;
	}
}

void STWBScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_isDrawing)
	{
		onPenMove(event->scenePos().toPoint());
		if (m_realtimePoints.size() == 10)
		{
			QVector<QPoint> tmpPoints = m_realtimePoints;
			drawRealtimePenItem(m_pen_color, m_pen_thickness, tmpPoints);
			m_realtimePoints.clear();
		}
		m_realtimePoints.append(event->scenePos().toPoint());
	}
	QGraphicsScene::mouseMoveEvent(event);
}

void STWBScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_isDrawing)
	{
		QVector<QPoint> tmpPoints = m_realtimePoints;
		drawRealtimePenItem(m_pen_color, m_pen_thickness, tmpPoints);
		m_realtimePoints.clear();
		m_isDrawing = false;
		onPenUp();
	}
	if (m_type == ActionType::AT_Select)
	{
		QMap<QString, QGraphicsItem*>::iterator it;
		QGraphicsItem* item;
		QString itemID;
		for (int i = 0; i < selectedItems().size(); i++)
		{
			item = selectedItems().at(i);
			for (it = m_itemMap.begin(); it != m_itemMap.end(); it++)
			{
				if (it.value() == item)
				{
					itemID = it.key();
					break;
				}
			}
			moveItem(item->pos().toPoint(), itemID);
		}
	}
	QGraphicsScene::mouseReleaseEvent(event);
}

void STWBScene::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Delete)
	{
		deleteSelectedItem();
	}
	else
	{
		QGraphicsScene::keyPressEvent(event);
	}
}

void STWBScene::drawLocalTextItem(QString content, QPoint pos, QString itemID)
{
	drawTextItem(m_text_color, m_text_size, content, pos, itemID);
}

void STWBScene::deleteSelectedItem()
{
	// 移除所有选中的 items
	int itemID = 0;
	QGraphicsItem* item;
	QMap<QString, QGraphicsItem*>::iterator it;
	QList<QString> itemIDs;
	while (!selectedItems().isEmpty())
	{
		item = selectedItems().front();
		for (it = m_itemMap.begin(); it != m_itemMap.end(); it++)
		{
			if (it.value() == item)
			{
				itemIDs.append(it.key());
				break;
			}
		}
		removeItem(item);
	}
	deleteItems(itemIDs);
}

void STWBScene::drawRealtimePenItem(QString penColor, int penThickness, QVector<QPoint> points)
{
	// {"type":"wbrealtime","courseID":"111","data":{"action":"pen","color":"#000000","thickness":12,"points":[12,23,12,34]}}
	QJsonObject complexJson;
	complexJson.insert("type", "wbrealtime");
	complexJson.insert("courseID", m_courseID);

	QJsonObject dataJson;
	dataJson.insert("action", "pen");
	dataJson.insert("color", penColor);
	dataJson.insert("thickness", penThickness);

	QJsonArray pointsArray;
	for (int i = 0, j = 0; i < points.size(); i++, j += 2)
	{
		pointsArray.insert(j, points[i].x());
		pointsArray.insert(j + 1, points[i].y());
	}
	dataJson.insert("points", pointsArray);

	complexJson.insert("data", dataJson);

	QJsonDocument complexDocument;
	complexDocument.setObject(complexJson);
	QByteArray complexByteArray = complexDocument.toJson(QJsonDocument::Compact);
	QString complexJsonStr(complexByteArray);

	m_messageClient->sendMessage(complexJsonStr);
}

void STWBScene::drawPenItem(QString penColor, int penThickness, QVector<QPoint> points, QString itemID)
{
	// {"type":"wbitem","subtype":"add","courseID":"111","data":{"action":"pen","color":"#000000","thickness":12,"points":[12,23,12,34]},"id":"uuid"}
	QJsonObject complexJson;
	complexJson.insert("type", "wbitem");
	complexJson.insert("subtype", "add");
	complexJson.insert("courseID", m_courseID);
	complexJson.insert("id", itemID);

	QJsonObject dataJson;
	dataJson.insert("action", "pen");
	dataJson.insert("color", penColor);
	dataJson.insert("thickness", penThickness);

	QJsonArray pointsArray;
	for (int i = 0, j = 0; i < points.size(); i++, j += 2)
	{
		pointsArray.insert(j, points[i].x());
		pointsArray.insert(j + 1, points[i].y());
	}
	dataJson.insert("points", pointsArray);

	complexJson.insert("data", dataJson);

	QJsonDocument complexDocument;
	complexDocument.setObject(complexJson);
	QByteArray complexByteArray = complexDocument.toJson(QJsonDocument::Compact);
	QString complexJsonStr(complexByteArray);

	m_messageClient->sendMessage(complexJsonStr);
}

void STWBScene::drawTextItem(QString textColor, int textSize,
	QString content, QPoint position, QString itemID)
{
	// {"type":"wbitem","subtype":"add","courseID":"111","data":{"action":"text","color":"#000000","size":12,"content":"aaa","pos":[12,23]},"id":"uuid"}
	QJsonObject complexJson;
	complexJson.insert("type", "wbitem");
	complexJson.insert("subtype", "add");
	complexJson.insert("courseID", m_courseID);
	complexJson.insert("id", itemID);

	QJsonObject dataJson;
	dataJson.insert("action", "text");
	dataJson.insert("color", textColor);
	dataJson.insert("size", textSize);
	dataJson.insert("content", content);

	QJsonArray pointsArray;
	pointsArray.insert(0, position.x());
	pointsArray.insert(1, position.y());
	dataJson.insert("pos", pointsArray);

	complexJson.insert("data", dataJson);

	QJsonDocument complexDocument;
	complexDocument.setObject(complexJson);
	QByteArray complexByteArray = complexDocument.toJson(QJsonDocument::Compact);
	QString complexJsonStr(complexByteArray);

	m_messageClient->sendMessage(complexJsonStr);
}

void STWBScene::moveItem(QPoint position, QString itemID)
{
	// {"type":"wbitem","subtype":"move","courseID":"111","data":{"pos":[2,3]},"id":"uuid"}
	QJsonObject complexJson;
	complexJson.insert("type", "wbitem");
	complexJson.insert("subtype", "move");
	complexJson.insert("courseID", m_courseID);
	complexJson.insert("id", itemID);

	QJsonObject dataJson;
	QJsonArray posArray;
	posArray.insert(0, position.x());
	posArray.insert(1, position.y());
	dataJson.insert("pos", posArray);
	complexJson.insert("data", dataJson);

	QJsonDocument complexDocument;
	complexDocument.setObject(complexJson);
	QByteArray complexByteArray = complexDocument.toJson(QJsonDocument::Compact);
	QString complexJsonStr(complexByteArray);

	m_messageClient->sendMessage(complexJsonStr);
}

void STWBScene::deleteItems(QList<QString> itemIDs)
{
	// {"type":"wbitem","subtype":"del","courseID":"111","ids":["uuid1","uuid2","uuid3"]}
	QJsonObject complexJson;
	complexJson.insert("type", "wbitem");
	complexJson.insert("subtype", "del");
	complexJson.insert("courseID", m_courseID);

	QJsonArray itemIDArray;
	for (int i = 0; i < itemIDs.size(); i++)
	{
		itemIDArray.insert(i, itemIDs[i]);
	}
	complexJson.insert("ids", itemIDArray);

	QJsonDocument complexDocument;
	complexDocument.setObject(complexJson);
	QByteArray complexByteArray = complexDocument.toJson(QJsonDocument::Compact);
	QString complexJsonStr(complexByteArray);

	m_messageClient->sendMessage(complexJsonStr);
}

void STWBScene::clearStatus()
{
	clearSelection();
	removeEmptyTextItem();
}

void STWBScene::removeEmptyTextItem()
{
	if (m_textItem != NULL && m_textItem->toPlainText().length() == 0)
	{
		QGraphicsScene::removeItem(m_textItem);
		m_textItem = NULL;
	}
}

void STWBScene::drawRemoteRealtimePen(QString color, int thickness, QVector<QPoint> points)
{
	if (points.size() == 0)
	{
		return;
	}
	if (m_realtimePathItemData == NULL)
	{
		m_realtimePathItemData = new PathItemData;
		m_realtimePathItemData->prePoint = points[0];
		drawStart(m_realtimePathItemData);
		drawRemoteRealtimePen(color, thickness, points);
	}
	else
	{
		for (int i = 0; i < points.size(); i++)
		{
			drawTo(m_realtimePathItemData, points[i]);
			m_realtimePathItemData->prePoint = points[i];
		}
	}
}

void STWBScene::drawRemotePenItem(QString color, int thickness, QVector<QPoint> points, QString itemID)
{
	m_remotePathItem = new STWBPathItem(itemID);
	m_remotePathItem->setColor(color);
	m_remotePathItem->setThickness(thickness);
	for (int i = 0; i < points.size(); i++)
	{
		m_remotePathItem->addPoint(points[i]);
	}
	m_remotePathItem->render();
	QGraphicsScene::addItem(m_remotePathItem);
	m_itemMap[itemID] = m_remotePathItem;

	if (m_realtimePathItemData != NULL)
	{
		for (int i = 0; i < m_realtimePathItemData->tempDrawingItem.size(); i++)
		{
			QGraphicsScene::removeItem(m_realtimePathItemData->tempDrawingItem[i]);
		}

		m_realtimePathItemData->tempDrawingItem.clear();
		delete(m_realtimePathItemData);
		m_realtimePathItemData = NULL;
	}
}

void STWBScene::drawRemoteTextItem(QString color, int size, QString content, QPoint pos, QString itemID)
{
	if (m_itemMap.contains(itemID))
	{
		m_remoteTextItem = (STWBTextItem *)m_itemMap[itemID];
	}
	else
	{
		m_remoteTextItem = new STWBTextItem(itemID);
		QGraphicsScene::addItem(m_remoteTextItem);
		m_itemMap[itemID] = m_remoteTextItem;
	}
	m_remoteTextItem->setDefaultTextColor(color);
	m_remoteTextItem->setPlainText(content);
	QFont font = m_remoteTextItem->font();
	font.setPixelSize(size);
	m_remoteTextItem->setFont(font);
	m_remoteTextItem->setPos(pos);
}

void STWBScene::moveRemoteItem(QPoint pos, QString itemID)
{
	m_itemMap[itemID]->setPos(pos);
}

void STWBScene::deleteRemoteItems(QList<QString> itemIDs)
{
	for (int i = 0; i < itemIDs.size(); i++)
	{
		QGraphicsScene::removeItem(m_itemMap[itemIDs[i]]);
		m_itemMap.remove(itemIDs[i]);
	}
}
