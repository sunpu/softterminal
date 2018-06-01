#include "stchatdetail.h"

using namespace tahiti;

STChatDetail::STChatDetail(XmppClient* client, QWidget *parent)
	: QWidget(parent), m_xmppClient(client), m_main(parent)
{
	ui.setupUi(this);

	ui.spChatDetail->setStretchFactor(0, 10);
	ui.spChatDetail->setStretchFactor(1, 1);

	ui.teChatWrite->installEventFilter(this);
	ui.lwChatRecordList->installEventFilter(this);

	m_option = new STScreenshotOption(this);
	connect(m_option, SIGNAL(screenshot()), this, SLOT(onScreenshot()));
	connect(m_option, SIGNAL(screenshotWithoutWindow()), this, SLOT(onScreenshotWithoutWindow()));
	m_option->hide();

	m_emotion = new STEmotion(this);
	connect(m_emotion, SIGNAL(chooseEmotion(int)), this, SLOT(onChooseEmotion(int)));
	m_emotion->hide();
	m_group = NULL;

	m_scrollbar = ui.lwChatRecordList->verticalScrollBar();
	connect(m_scrollbar, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));

	m_scrollNum = 0;

	m_movie = new QMovie(":/SoftTerminal/images/load_small.gif");
	ui.lblLoad->setMovie(m_movie);
	ui.lblLoad->setVisible(false);
	connect(this, SIGNAL(showMoreRecordSignal()), this, SLOT(showMoreRecord()));
}

STChatDetail::~STChatDetail()
{

}

UserInfo STChatDetail::getUserInfo()
{
	return m_userInfo;
}

void STChatDetail::clearChatDetail()
{
	ui.lwChatRecordList->clear();
	ui.teChatWrite->clear();
	m_recordItemList.clear();
	m_recordList.clear();
	ui.pbLoadMore->setVisible(false);
	ui.widLoadMore->setVisible(true);
}

void STChatDetail::setChatDetail(UserInfo userInfo, XmppGroup* group)
{
	if (m_group)
	{
		disconnect(m_group, SIGNAL(refreshOnlineSignal()), this, SLOT(refreshOnlineSlot()));
	}
	m_userInfo = userInfo;
	m_group = group;
	ui.widStatus->setVisible(false);
	if (m_group)
	{
		connect(m_group, SIGNAL(refreshOnlineSignal()), this, SLOT(refreshOnlineSlot()));
		refreshOnlineSlot();
		ui.widStatus->setVisible(true);
	}
	m_selfInfo = m_xmppClient->getSelfInfo();

	clearChatDetail();

	STRecordManager* recordManager = new STRecordManager(userInfo.jid);
	m_recordList = recordManager->getRecordItemList();

	showMoreRecord();

	ui.lwChatRecordList->scrollToBottom();
}

void STChatDetail::on_pbLoadMore_clicked()
{
	ui.pbLoadMore->setVisible(false);
	ui.lblLoad->setVisible(true);
	m_movie->start();
	pthread_create(&m_tidLoad, NULL, loadProc, this);
}

void* STChatDetail::loadProc(void* args)
{
	QThread::sleep(1);
	STChatDetail* chatDetail = (STChatDetail*)args;
	chatDetail->loadMoreRecord();
	return NULL;
}

void STChatDetail::loadMoreRecord()
{
	Q_EMIT showMoreRecordSignal();
}

void STChatDetail::showMoreRecord()
{
	int unshowNum = 0;
	int itemCount = m_recordList.size();
	RecordItem item;
	QSize itemSize;
	STChatRecordItem* chatDetailItem;
	QListWidgetItem* pItem;
	for (size_t i = 0; i < 12; i++)
	{
		if (m_recordList.isEmpty())
		{
			ui.pbLoadMore->setVisible(false);
			break;
		}
		item = m_recordList.takeLast();

		chatDetailItem = new STChatRecordItem(item);
		m_recordItemList.append(chatDetailItem);
		itemSize = chatDetailItem->getItemSize();
		pItem = new QListWidgetItem();
		pItem->setSizeHint(QSize(0, itemSize.height() + 56));
		ui.lwChatRecordList->insertItem(0, pItem);
		ui.lwChatRecordList->setItemWidget(pItem, chatDetailItem);
		unshowNum++;
	}
	m_scrollbar->setSliderPosition(unshowNum);
	ui.lblLoad->setVisible(false);
	m_movie->stop();
}

void STChatDetail::refreshOnlineSlot()
{
	ui.lwStatus->clear();

	STContactItem* contactItem;
	QListWidgetItem* item;

	QString owner = m_group->getOwner();
	QList<QString> offlines = m_group->getMembers();
	QList<QString> onlines = m_group->getOnlines();

	QString num = QStringLiteral("  群组成员: %1/%2").arg(
		QString::number(onlines.size()), QString::number(offlines.size() + 1));
	ui.lblNum->setText(num);

	contactItem = new STContactItem(true, false, false, true);
	bool offline = !onlines.contains(owner.split("@localhost")[0]);
	contactItem->setUserInfo(m_group->getUserInfo(owner), offline);
	item = new QListWidgetItem();
	ui.lwStatus->addItem(item);
	ui.lwStatus->setItemWidget(item, contactItem);

	QList<QString>::Iterator it;
	QString jid;
	for (it = onlines.begin(); it != onlines.end(); it++)
	{
		jid = it->append("@localhost");
		if (owner == jid)
		{
			continue;
		}
		if (offlines.contains(jid))
		{
			offlines.removeOne(jid);

			contactItem = new STContactItem(true, false, false);
			contactItem->setUserInfo(m_group->getUserInfo(jid));
			item = new QListWidgetItem();
			ui.lwStatus->addItem(item);
			ui.lwStatus->setItemWidget(item, contactItem);
		}
	}

	for (it = offlines.begin(); it != offlines.end(); it++)
	{
		contactItem = new STContactItem(true, false, false);
		contactItem->setUserInfo(m_group->getUserInfo(*it), true);
		item = new QListWidgetItem();
		ui.lwStatus->addItem(item);
		ui.lwStatus->setItemWidget(item, contactItem);
	}
}

void STChatDetail::onSliderChanged(int pos)
{
	if (pos == 0 && !m_recordList.isEmpty())
	{
		ui.pbLoadMore->setVisible(true);
	}
	else if (m_recordList.isEmpty())
	{
		ui.widLoadMore->setVisible(false);
	}
	else
	{
		ui.pbLoadMore->setVisible(false);
	}
}

void STChatDetail::updateSelfPic(QString picPath)
{
	m_selfInfo.photoPath = picPath;
	QList<STChatRecordItem*>::Iterator it;
	for (it = m_recordItemList.begin(); it != m_recordItemList.end(); it++)
	{
		(*it)->updateSelfPic(picPath);
	}
}

void STChatDetail::on_pbSendMessage_clicked()
{
	QString myMessage = ui.teChatWrite->toPlainText();
	if (myMessage.size() == 0)
	{
		ui.teChatWrite->setFocus();
		QPoint pos(ui.teChatWrite->mapToGlobal(ui.teChatWrite->pos()).x(),
			ui.teChatWrite->mapToGlobal(ui.teChatWrite->pos()).y());
		QToolTip::showText(pos, QStringLiteral("未输入信息"), ui.teChatWrite);
		return;
	}

	RecordItem item;
	item.time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	item.from = MessageFrom::Self;
	item.jid = m_selfInfo.jid;
	item.type = MessageType::MT_Text;
	item.content = myMessage;
	item.pic = m_selfInfo.photoPath;

	// 写文件
	STRecordManager* recordManager = new STRecordManager(m_userInfo.jid);
	recordManager->writeRecordItem(item);

	// 更新聊天记录界面
	STChatRecordItem* chatDetailItem = new STChatRecordItem(item);
	m_recordItemList.append(chatDetailItem);
	QSize itemSize = chatDetailItem->getItemSize();
	QListWidgetItem* pItem = new QListWidgetItem();
	pItem->setSizeHint(QSize(ui.lwChatRecordList->width() - 5, itemSize.height() + 56));
	ui.lwChatRecordList->addItem(pItem);

	ui.lwChatRecordList->setItemWidget(pItem, chatDetailItem);

	m_scrollbar->setSliderPosition(m_scrollbar->maximum());

	// 清空输入框
	ui.teChatWrite->clear();
	Q_EMIT changeChatListOrder(m_userInfo.jid);

	// 发送消息给远端
	if (m_group)
	{
		m_group->sendMsg(myMessage);
	}
	else
	{
		m_xmppClient->sendMsg(m_userInfo.jid, myMessage);
	}
}

void STChatDetail::on_pbEmotion_clicked()
{
	m_emotion->hide();
	int x = ui.pbEmotion->mapToGlobal(ui.pbEmotion->pos()).x() - m_emotion->width() / 2 + ui.pbEmotion->width();
	int y = ui.pbEmotion->mapToGlobal(ui.pbEmotion->pos()).y() - m_emotion->height() - ui.pbEmotion->height();
	m_emotion->move(QPoint(x, y));
	m_emotion->show();
}

void STChatDetail::on_pbScreenShot_clicked()
{
	openScreenshot();
}

void STChatDetail::on_pbScreenShotOption_clicked()
{
	m_option->hide();
	int x = ui.pbScreenShotOption->mapToGlobal(ui.pbScreenShotOption->pos()).x() - 50;
	int y = ui.pbScreenShotOption->mapToGlobal(ui.pbScreenShotOption->pos()).y();
	m_option->move(QPoint(x, y));
	m_option->show();
}

void STChatDetail::on_pbLesson_clicked()
{
	m_whiteboard = new STWhiteBoard(m_selfInfo.jid, m_selfInfo.userName);

	connect(m_main, SIGNAL(closeMain()), m_whiteboard, SLOT(on_pbClose_clicked()));
	m_whiteboard->show();

	/*RecordItem item;
	item.time = QTime::currentTime().toString();
	item.from = MessageFrom::Self;
	item.type = MessageType::MT_Class;
	item.content = myMessage;
	item.pic = m_selfPicPath;

	// 写文件
	STRecordManager* recordManager = new STRecordManager(m_userInfo.jid);
	recordManager->writeRecordItem(item);

	// 更新聊天记录界面
	STChatRecordItem* chatDetailItem = new STChatRecordItem(item);
	m_recordItemList.append(chatDetailItem);
	QSize itemSize = chatDetailItem->getItemSize();
	QListWidgetItem* pItem = new QListWidgetItem();
	pItem->setSizeHint(QSize(ui.lwChatRecordList->width() - 5, itemSize.height() + 56));
	ui.lwChatRecordList->addItem(pItem);

	ui.lwChatRecordList->setItemWidget(pItem, chatDetailItem);

	m_scrollbar->setSliderPosition(m_scrollbar->maximum());

	Q_EMIT changeChatListOrder(m_userInfo.jid);

	// 发送消息给远端
	m_xmppClient->sendMsg(m_userInfo.jid, myMessage);*/
}

void STChatDetail::openScreenshot()
{
	STScreenshot* ss = new STScreenshot();
	connect(ss, SIGNAL(finishScreenshot(QString)), this, SLOT(onFinishScreenshot(QString)));
	connect(ss, SIGNAL(cancelScreenshot()), this, SLOT(onCancelScreenshot()));
	ss->show();
}

void STChatDetail::onScreenshot()
{
	m_option->hide();
	openScreenshot();
}

void STChatDetail::onScreenshotWithoutWindow()
{
	m_main->showMinimized();
	openScreenshot();
}

void STChatDetail::onChooseEmotion(int index)
{
	QString path = QString(":/SoftTerminal/emoji/%1.jpg").arg(index);
	QImage image(path);

	QTextCursor cursor = ui.teChatWrite->textCursor();
	QTextDocument* document = ui.teChatWrite->document();
	//cursor.movePosition(QTextCursor::End);

	document->addResource(QTextDocument::ImageResource, QUrl(path), QVariant(image));

	QTextImageFormat image_format;
	image_format.setName(path);
	cursor.insertImage(image_format);
}

void STChatDetail::onFinishScreenshot(QString path)
{
	m_main->showNormal();
	QImage image(path);

	QTextCursor cursor = ui.teChatWrite->textCursor();
	QTextDocument* document = ui.teChatWrite->document();
	//cursor.movePosition(QTextCursor::End);

	document->addResource(QTextDocument::ImageResource, QUrl(path), QVariant(image));

	QTextImageFormat image_format;
	image_format.setName(path);
	image_format.setWidth(80);
	image_format.setHeight(60);
	cursor.insertImage(image_format);
}

void STChatDetail::onCancelScreenshot()
{
	m_main->showNormal();
}

void STChatDetail::updateOthersMessage(RecordItem item)
{
	// 更新聊天记录界面
	STChatRecordItem* chatDetailItem = new STChatRecordItem(item);
	m_recordItemList.append(chatDetailItem);
	QSize itemSize = chatDetailItem->getItemSize();
	QListWidgetItem* pItem = new QListWidgetItem();
	ui.lwChatRecordList->addItem(pItem);
	ui.lwChatRecordList->setItemWidget(pItem, chatDetailItem);
	pItem->setSizeHint(QSize(ui.lwChatRecordList->width() - 5, itemSize.height() + 56));

	m_scrollbar->setSliderPosition(m_scrollbar->maximum());
}

bool STChatDetail::eventFilter(QObject *obj, QEvent *e)
{
	if (e->type() == QEvent::KeyPress && ui.teChatWrite == obj)
	{
		QKeyEvent *event = static_cast<QKeyEvent*>(e);
		if ((event->key() == Qt::Key_Return
			|| event->key() == Qt::Key_Enter) && (event->modifiers() & Qt::ControlModifier))
		{
			ui.teChatWrite->insertPlainText("\n");
			return true;
		}
		else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
		{
			on_pbSendMessage_clicked();
			return true;
		}
	}
	else if (e->type() == QEvent::Wheel && ui.lwChatRecordList == obj)
	{
		m_scrollNum++;
		if (m_scrollbar->sliderPosition() == 0 && m_scrollNum > 5 && m_scrollbar->isVisible())
		{
			on_pbLoadMore_clicked();
			m_scrollNum = 0;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
STScreenshotOption::STScreenshotOption(QWidget * parent) : QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Popup);

	ui.widScreenshot->installEventFilter(this);
	ui.widScreenshotWithoutWindow->installEventFilter(this);
}

STScreenshotOption::~STScreenshotOption()
{

}

bool STScreenshotOption::eventFilter(QObject* obj, QEvent* e)
{
	if (e->type() == QEvent::Enter)
	{
		if (ui.widScreenshot == obj)
		{
			ui.widScreenshot->setStyleSheet("background:#ececec;");
		}
		else if (ui.widScreenshotWithoutWindow == obj)
		{
			ui.widScreenshotWithoutWindow->setStyleSheet("background:#ececec;");
		}
		return true;
	}
	else if (e->type() == QEvent::Leave)
	{
		if (ui.widScreenshot == obj)
		{
			ui.widScreenshot->setStyleSheet("");
		}
		else if (ui.widScreenshotWithoutWindow == obj)
		{
			ui.widScreenshotWithoutWindow->setStyleSheet("");
		}
		return true;
	}
	else if (e->type() == QEvent::MouseButtonPress)
	{
		if (ui.widScreenshot == obj)
		{
			Q_EMIT screenshot();
		}
		else if (ui.widScreenshotWithoutWindow == obj)
		{
			Q_EMIT screenshotWithoutWindow();
		}
		return true;
	}
	return false;
}

bool STScreenshotOption::event(QEvent* event)
{
	// class_ameneded 不能是custommenu的成员, 因为winidchange事件触发时, 类成员尚未初始化  
	static bool class_amended = false;
	if (event->type() == QEvent::WinIdChange)
	{
		HWND hwnd = reinterpret_cast<HWND>(winId());
		if (class_amended == false)
		{
			class_amended = true;
			DWORD class_style = ::GetClassLong(hwnd, GCL_STYLE);
			class_style &= ~CS_DROPSHADOW;
			::SetClassLong(hwnd, GCL_STYLE, class_style); // windows系统函数  
		}
	}
	return QWidget::event(event);
}
