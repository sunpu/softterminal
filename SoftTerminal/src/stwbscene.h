#ifndef _STWBSCENE_H
#define _STWBSCENE_H

#include <QPoint>
#include <QGraphicsScene>
#include <QObject>
#include <QMap>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include <QGraphicsProxyWidget>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUuid>
#include "stwbpathitem.h"
#include "stwbtextitem.h"
#include "stmessageclient.h"

namespace tahiti
{
	struct PathItemData
	{
		QPoint prePoint;
		std::vector<QGraphicsItem*> tempDrawingItem;
		STWBPathItem* pathItem;
		PathItemData()
		{
			pathItem = Q_NULLPTR;
			tempDrawingItem.clear();
		}
	};

	class STWBTextItem;
	class STWBScene : public QGraphicsScene
	{
	public:
		STWBScene();
		void setCourseID(QString courseID);
		void setMode(ActionType type);
		void setPenColor(QString color);
		void setPenThickness(int w);
		void setTextColor(QString color);
		void setTextSize(int size);
		void onPenDown(QPoint pt, int id = 0);
		void onPenMove(QPoint pt, int id = 0);
		void onPenUp(int id = 0);
		void deleteSelectedItem();
		void clearStatus();
		void drawRemoteRealtimePen(QString color, int thickness, QVector<QPoint> points);
		void drawLocalTextItem(QString content, QPoint pos, QString itemID);
		void drawRemotePenItem(QString color, int thickness, QVector<QPoint> points, QString itemID);
		void drawRemoteTextItem(QString color, int size, QString content, QPoint pos, QString itemID);
		void moveRemoteItem(QPoint pos, QString itemID);
		void deleteRemoteItems(QList<QString> itemIDs);
	private:
		void drawRealtimePenItem(QString penColor, int penThickness, QVector<QPoint> points);
		void drawPenItem(QString penColor, int penThickness, QVector<QPoint> points, QString itemID);
		void drawTextItem(QString textColor, int textSize, QString content, QPoint position, QString itemID);
		void moveItem(QPoint position, QString itemID);
		void deleteItems(QList<QString> itemIDs);
	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent* event);
		void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
		void keyPressEvent(QKeyEvent* event);
		void drawStart(PathItemData* dt);
		void drawTo(PathItemData* dt, const QPoint& to);
	private:
		void removeEmptyTextItem();
	private:
		ActionType m_type;
		QString m_pen_color;
		int m_pen_thickness;
		QString m_text_color;
		int m_text_size;
		bool m_isDrawing;
		PathItemData* m_pathItemData;
		STWBTextItem* m_textItem;
		STWBTextItem* m_last_textItem;
		PathItemData* m_realtimePathItemData;
		STWBPathItem* m_remotePathItem;
		STWBTextItem* m_remoteTextItem;
		QMap<QString, QGraphicsItem*> m_itemMap;
		QVector<QPoint> m_realtimePoints;
		STMessageClient* m_messageClient;
		QString m_courseID;
	};
}
#endif
