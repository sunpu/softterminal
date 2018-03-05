#include "stchatdetail.h"

using namespace tahiti;

STChatDetail::STChatDetail(XmppClient* client, QWidget *parent)
	: QWidget(parent), m_xmppClient(client), m_main(parent)
{
	ui.setupUi(this);

	ui.spChatDetail->setStretchFactor(0, 10);
	ui.spChatDetail->setStretchFactor(1, 1);

	connect(m_xmppClient, SIGNAL(showMessage(QString, QString)), this, SLOT(updateOthersMessage(QString, QString)));

	ui.teChatWrite->installEventFilter(this);

	m_option = new STScreenshotOption(this);
	connect(m_option, SIGNAL(screenshot()), this, SLOT(onScreenshot()));
	connect(m_option, SIGNAL(screenshotWithoutWindow()), this, SLOT(onScreenshotWithoutWindow()));
	m_option->hide();

	m_emotion = new STEmotion(this);
	connect(m_emotion, SIGNAL(chooseEmotion(int)), this, SLOT(onChooseEmotion(int)));
	m_emotion->hide();
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
}

void STChatDetail::setChatDetail(UserInfo userInfo)
{
	m_userInfo = userInfo;
	m_selfInfo = m_xmppClient->getSelfInfo();

	QString desPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
		+ DATA_ROOT_PATH + AVATAR_PATH;
	QDir dir;
	dir.setPath(desPath);
	QDirIterator iter(dir, QDirIterator::Subdirectories);
	while (iter.hasNext())
	{
		iter.next();
		QFileInfo info = iter.fileInfo();
		if (info.isFile() && info.fileName().startsWith(m_selfInfo.jid))
		{
			m_selfPicPath = QString(desPath) + info.fileName();
		}
		if (info.isFile() && info.fileName().startsWith(userInfo.jid))
		{
			m_otherPicPath = QString(desPath) + info.fileName();
		}
	}

	clearChatDetail();

	STRecordManager* recordManager = new STRecordManager(userInfo.jid);
	QList<RecordItem> list = recordManager->getRecordItemList();

	QSize itemSize;
	QList<RecordItem>::Iterator it;
	for (it = list.begin(); it != list.end(); it++)
	{
		if (it->from == MessageFrom::Self)
		{
			it->pic = m_selfPicPath;
		}
		else if (it->from == MessageFrom::Other)
		{
			it->pic = m_otherPicPath;
		}
		STChatRecordItem* chatDetailItem = new STChatRecordItem(*it);
		m_recordItemList.append(chatDetailItem);
		itemSize = chatDetailItem->getItemSize();
		QListWidgetItem* pItem = new QListWidgetItem();
		//pItem->setSizeHint(itemSize);
		pItem->setSizeHint(QSize(ui.lwChatRecordList->width(), itemSize.height() + 56));
		ui.lwChatRecordList->addItem(pItem);
		ui.lwChatRecordList->setItemWidget(pItem, chatDetailItem);
	}
	ui.lwChatRecordList->scrollToBottom();

	m_selfInfo = m_xmppClient->getSelfInfo();
}

void STChatDetail::updateSelfPic(QString picPath)
{
	m_userInfo.photoPath = picPath;
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
		/*QPoint pos(mapToGlobal(ui.pbSendMessage->pos()).x(),
			mapToGlobal(ui.pbSendMessage->pos()).y());
		QToolTip::showText(pos, "aaa", ui.pbSendMessage);*/
		return;
	}

	RecordItem item;
	item.time = QTime::currentTime().toString();
	item.from = MessageFrom::Self;
	item.type = MessageType::MT_Text;
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
	ui.lwChatRecordList->scrollToBottom();

	// 清空输入框
	ui.teChatWrite->clear();
	Q_EMIT changeChatListOrder(m_userInfo.jid);

	// 发送消息给远端
	m_xmppClient->sendMsg(m_userInfo.jid, myMessage);
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
	m_whiteboard = new STWhiteBoard;
	m_whiteboard->show();
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

void STChatDetail::updateOthersMessage(QString jid, QString msg)
{
	if (!m_userInfo.jid.contains(jid))
	{
		return;
	}

	RecordItem item;
	item.time = QTime::currentTime().toString();
	item.from = MessageFrom::Other;
	item.type = MessageType::MT_Text;
	item.content = msg;
	item.pic = m_otherPicPath;

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
	ui.lwChatRecordList->scrollToBottom();
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
	else if (e->type() == QEvent::Resize)
	{
		ui.lwChatRecordList->scrollToBottom();
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
