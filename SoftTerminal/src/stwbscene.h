#ifndef STWBSCENE_H
#define STWBSCENE_H

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
#include "stwbpathitem.h"
#include "stwbtextitem.h"
#include "stnetworkclient.h"

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
		STWBScene(STNetworkClient* network);

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
		void drawLocalTextItem(QString content, QPoint pos, int itemID);
		void drawRemotePenItem(QString color, int thickness, QVector<QPoint> points, int itemID);
		void drawRemoteTextItem(QString color, int size, QString content, QPoint pos, int itemID);
		void moveRemoteItems(QPoint pos, int itemID);
		void deleteRemoteItems(QList<int> itemIDs);
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
		STNetworkClient* m_network;
		int m_itemID_index;
		PathItemData* m_realtimePathItemData;
		STWBPathItem* m_remotePathItem;
		STWBTextItem* m_remoteTextItem;
		QMap<int, QGraphicsItem*> m_itemMap;
		QVector<QPoint> m_realtimePoints;
	};
}
#endif
