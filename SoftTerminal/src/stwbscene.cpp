#include "stwbscene.h"

using namespace tahiti;

STWBScene::STWBScene(STNetworkClient* network)
	: m_network(network)
{
	m_isDrawing = false;

	m_realtimePathItemData = NULL;
	m_pathItemData = NULL;
	m_textItem = NULL;
	m_last_textItem = NULL;

	m_itemID_index = 0;
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
	m_pathItemData = new PathItemData;
	m_pathItemData->prePoint = pt;//scene point;
	m_pathItemData->pathItem = new STWBPathItem(m_itemID_index++);
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

	m_network->drawPenItem(m_pen_color, m_pen_thickness,
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
		/*QGraphicsPixmapItem* pixmapitem = new QGraphicsPixmapItem();
		pixmapitem->setFlag(QGraphicsItem::ItemIsMovable, true);
		pixmapitem->setFlag(QGraphicsItem::ItemIsSelectable, true);
		QImage* image = new QImage(":/PaintLearn/Resources/4.png");
		pixmapitem->setPixmap(QPixmap::fromImage(*image).scaled(80, 80));
		pixmapitem->setPos(event->scenePos().toPoint());
		pixmapitem->setSelected(true);
		pixmapitem->setFocus();
		QGraphicsScene::addItem(pixmapitem);*/


		// 创建 widget
		/*QLabel *pPixmapLabel = new QLabel();
		QLineEdit *pAccountLineEdit = new QLineEdit();
		QLineEdit *pPasswdLineEdit = new QLineEdit();
		QCheckBox *pRememberCheckBox = new QCheckBox();
		QCheckBox *pAutoLoginCheckBox = new QCheckBox();
		QPushButton *pLoginButton = new QPushButton();
		QPushButton *pRegisterButton = new QPushButton();
		QPushButton *pForgotButton = new QPushButton();

		pPixmapLabel->setStyleSheet("border-image: url(:/Images/logo); min-width:90px; min-height:90px; border-radius:45px; background:transparent;");
		pAccountLineEdit->setPlaceholderText(QStringLiteral("QQ号码/手机/邮箱"));
		pPasswdLineEdit->setPlaceholderText(QStringLiteral("密码"));
		pPasswdLineEdit->setEchoMode(QLineEdit::Password);
		pRememberCheckBox->setText(QStringLiteral("记住密码"));
		pAutoLoginCheckBox->setText(QStringLiteral("自动登录"));
		pLoginButton->setText(QStringLiteral("登录"));
		pRegisterButton->setText(QStringLiteral("注册账号"));
		pForgotButton->setText(QStringLiteral("找回密码"));

		pLoginButton->setFixedHeight(30);
		pAccountLineEdit->setFixedWidth(180);

		// 添加 widget
		QGraphicsScene *pScene = new QGraphicsScene();
		QGraphicsProxyWidget *pPixmapWidget = pScene->addWidget(pPixmapLabel);
		QGraphicsProxyWidget *pAccountWidget = pScene->addWidget(pAccountLineEdit);
		QGraphicsProxyWidget *pPasswdWidget = pScene->addWidget(pPasswdLineEdit);
		QGraphicsProxyWidget *pRememberWidget = pScene->addWidget(pRememberCheckBox);
		QGraphicsProxyWidget *pAutoLoginWidget = pScene->addWidget(pAutoLoginCheckBox);
		QGraphicsProxyWidget *pLoginWidget = pScene->addWidget(pLoginButton);
		QGraphicsProxyWidget *pRegisterWidget = pScene->addWidget(pRegisterButton);
		QGraphicsProxyWidget *pForgotWidget = pScene->addWidget(pForgotButton);

		// 添加至网格布局中
		QGraphicsGridLayout *pLayout = new QGraphicsGridLayout();
		//QGraphicsLayoutItem* item = (QGraphicsLayoutItem*)pPixmapWidget;
		pLayout->addItem(pPixmapWidget, 0, 0, 3, 1);
		pLayout->addItem(pAccountWidget, 0, 1, 1, 2);
		pLayout->addItem(pRegisterWidget, 0, 4);
		pLayout->addItem(pPasswdWidget, 1, 1, 1, 2);
		pLayout->addItem(pForgotWidget, 1, 4);
		pLayout->addItem(pRememberWidget, 2, 1, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
		pLayout->addItem(pAutoLoginWidget, 2, 2, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
		pLayout->addItem(pLoginWidget, 3, 1, 1, 2);
		pLayout->setHorizontalSpacing(10);
		pLayout->setVerticalSpacing(10);
		pLayout->setContentsMargins(10, 10, 10, 10);

		QGraphicsWidget *pWidget = new QGraphicsWidget();
		pWidget->setFlag(QGraphicsItem::ItemIsMovable, true);
		pWidget->setFlag(QGraphicsItem::ItemIsSelectable, true);
		pWidget->setLayout(pLayout);
		QGraphicsScene::addItem(pWidget);
		return;*/







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
		m_textItem = new STWBTextItem(m_itemID_index++);
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
			m_network->drawRealtimePenItem(m_pen_color, m_pen_thickness, tmpPoints);
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
		m_network->drawRealtimePenItem(m_pen_color, m_pen_thickness, tmpPoints);
		m_realtimePoints.clear();
		m_isDrawing = false;
		onPenUp();
	}
	if (m_type == ActionType::AT_Select)
	{
		QMap<int, QGraphicsItem*>::iterator it;
		QGraphicsItem* item;
		int itemID = 0;
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
			m_network->moveItem(item->pos().toPoint(), itemID);
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

void STWBScene::drawLocalTextItem(QString content, QPoint pos, int itemID)
{
	m_network->drawTextItem(m_text_color, m_text_size, content, pos, itemID);
}

void STWBScene::deleteSelectedItem()
{
	// 移除所有选中的 items
	int itemID = 0;
	QGraphicsItem* item;
	QMap<int, QGraphicsItem*>::iterator it;
	QList<int> itemIDs;
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
	m_network->deleteItems(itemIDs);
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

void STWBScene::drawRemotePenItem(QString color, int thickness, QVector<QPoint> points, int itemID)
{
	m_itemID_index = itemID;
	m_remotePathItem = new STWBPathItem(m_itemID_index++);
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

void STWBScene::drawRemoteTextItem(QString color, int size, QString content, QPoint pos, int itemID)
{
	if (m_itemMap.contains(itemID))
	{
		m_remoteTextItem = (STWBTextItem *)m_itemMap[itemID];
	}
	else
	{
		m_itemID_index = itemID;
		m_remoteTextItem = new STWBTextItem(m_itemID_index++);
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

void STWBScene::moveRemoteItems(QPoint pos, int itemID)
{
	m_itemMap[itemID]->setPos(pos);
}

void STWBScene::deleteRemoteItems(QList<int> itemIDs)
{
	for (int i = 0; i < itemIDs.size(); i++)
	{
		QGraphicsScene::removeItem(m_itemMap[itemIDs[i]]);
		m_itemMap.remove(itemIDs[i]);
	}
}
