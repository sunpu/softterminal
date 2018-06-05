#ifndef _STWBVIEW_H
#define _STWBVIEW_H

#include <QWidget>
#include <QtOpenGL>
#include <QGraphicsView>
#include "stcommon.h"

namespace tahiti
{
	class STWBScene;
	class STWBView : public QGraphicsView
	{
		Q_OBJECT

	public:
		STWBView(bool subWin = false);
		~STWBView();

		void setCourseID(QString courseID);
		STWBScene* getScene() { return m_scene; }
		void setScene(STWBScene* scene);
		void setMode(ActionType type);
		void setPenColor(QString color);
		void setPenThickness(int width);
		void setTextColor(QString color);
		void setTextSize(int size);
		void deleteSelectedItem();
		void clearSelection();
		void drawRemoteRealtimePen(QString color, int thickness, QVector<QPoint> points);
		void drawRemotePenItem(QString color, int thickness, QVector<QPoint> points, QString itemID);
		void drawRemoteTextItem(QString color, int size, QString content, QPoint pos, QString itemID);
		void moveRemoteItem(QPoint pos, QString itemID);
		void deleteRemoteItems(QList<QString> itemIDs);
		void setBackground(QPixmap image, QSize size);
	protected:
		void resizeEvent(QResizeEvent* event);
	private:
		STWBScene* m_scene;
		QString m_courseID;
	};
}
#endif
