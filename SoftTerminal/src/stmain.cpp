#include "stmain.h"

using namespace tahiti;

STMain::STMain(XmppClient* client) : m_xmppClient(client)
{

	ui.setupUi(this);

	// 状态初始化
	this->setWindowFlags(Qt::FramelessWindowHint); //去掉标题栏
	setAttribute(Qt::WA_TranslucentBackground);
	ui.pbNormal->setVisible(false);

	// 初始化搜索框
	QHBoxLayout* mainLayout = new QHBoxLayout;
	QPushButton* searchBtn = new QPushButton;
	searchBtn->setFixedSize(13, 13);
	searchBtn->setCursor(Qt::PointingHandCursor);
	searchBtn->setStyleSheet("QPushButton{border-image:url(:/SoftTerminal/images/search.png);"
		"background:transparent;}");
	mainLayout->addWidget(searchBtn);
	mainLayout->addStretch();
	mainLayout->setContentsMargins(8, 0, 0, 0);
	ui.leContactSearch->setTextMargins(13 + 8 + 2, 0, 0, 0);
	ui.leContactSearch->setContentsMargins(0, 0, 0, 0);
	ui.leContactSearch->setLayout(mainLayout);
	ui.leContactSearch->setStyleSheet("border:1px solid #eaeaea;");

	// 初始化左侧工具栏
	ui.pbChat->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/chat_on.png);}");

	m_menu = new STMenu(this);
	connect(m_menu, SIGNAL(confirmExit()), this, SLOT(confirmExit()));
	connect(m_menu, SIGNAL(confirmRelogin()), this, SLOT(confirmRelogin()));
	connect(m_menu, SIGNAL(showSettingWindow()), this, SLOT(showSettingWindow()));
	
	m_menu->hide();
	m_confirm = new STConfirm(this);
	connect(m_confirm, SIGNAL(confirmOK()), this, SLOT(handleConfirmOK()));

	// 初始化右侧窗口
	initContactAddNew();

	m_contactDetail = new STContactDetail();
	ui.widContactDetail->layout()->addWidget(m_contactDetail);
	connect(m_contactDetail, SIGNAL(openChatDetail(QString)), this, SLOT(switchChatWindow(QString)));

	ui.lblDirect->installEventFilter(this);
	ui.lblFriend->installEventFilter(this);
	ui.widContactTitle->installEventFilter(this);
	ui.lblFriendNum->installEventFilter(this);
	ui.widTitle->installEventFilter(this);
	ui.lblUserPic->installEventFilter(this);

	m_personalInfo = new STPersonalInfo(m_xmppClient);
	connect(m_personalInfo, SIGNAL(updateSelfPic(QString)), this, SLOT(updateSelfPic(QString)));
	ui.pgPersonalInfo->layout()->addWidget(m_personalInfo);

	m_cloudFileManager = new STCloudFileManager();
	ui.pgCloudFileWindow->layout()->addWidget(m_cloudFileManager);
}

STMain::~STMain()
{
	delete m_contactDetail;
}

void STMain::initChatData()
{
	// 获得所有的聊天记录文件
	QString path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
		+ DATA_ROOT_PATH + RECORD_PATH;
	QDir dir(path);
	QStringList nameFilters;
	nameFilters << "*.dat";
	QStringList files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Time);

	// 获得好友列表
	QList<UserInfo> friendList = m_xmppClient->getRoster();
	QList<UserInfo>::const_iterator friendIt;

	STChatItem* chatItem;
	STChatDetail* chatDetail;
	QList<QString>::Iterator fileIt;
	// 过滤
	for (fileIt = files.begin(); fileIt != files.end(); fileIt++)
	{
		for (friendIt = friendList.constBegin(); friendIt != friendList.constEnd(); friendIt++)
		{
			if (fileIt->split(".")[0] == friendIt->jid)
			{
				chatItem = new STChatItem();
				chatItem->setUserInfo(*friendIt);
				m_chatItemList.push_back(chatItem);

				chatDetail = new STChatDetail(m_xmppClient, this);
				chatDetail->setChatDetail(*friendIt);
				connect(chatDetail, SIGNAL(changeChatListOrder(QString)), this, SLOT(reorderChatList(QString)));
				m_chatDetailList.push_back(chatDetail);

				ui.swChatDetail->addWidget(chatDetail);
			}
		}
	}
}

void STMain::initChatList()
{
	ui.lwChatList->clear();

	QListWidgetItem* item;
	QList<STChatItem*>::Iterator it;
	for (it = m_chatItemList.begin(); it != m_chatItemList.end(); it++)
	{
		item = new QListWidgetItem();
		ui.lwChatList->addItem(item);
		ui.lwChatList->setItemWidget(item, *it);
	}
}

void STMain::initChatMainWindow()
{
	ui.swChatDetail->setVisible(false);
	ui.widChatBlank->setVisible(true);
}

void STMain::initChatWindow()
{
	initChatData();
	initChatList();
	initChatMainWindow();
}

void STMain::reorderChatList(QString jid)
{
	int index = 0;
	STChatItem* chatItem;
	UserInfo userInfo;
	QList<STChatItem*>::Iterator it;
	for (it = m_chatItemList.begin(); it != m_chatItemList.end(); it++, index++)
	{
		if ((*it)->getUserInfo().jid == jid)
		{
			chatItem = *it;
			userInfo = (*it)->getUserInfo();
			break;
		}
	}

	STChatItem* chatItemNew = new STChatItem();
	chatItemNew->setUserInfo(userInfo);

	QListWidgetItem* item = ui.lwChatList->currentItem();
	ui.lwChatList->removeItemWidget(item);
	delete item;

	m_chatItemList.removeAt(index);
	m_chatItemList.push_front(chatItemNew);

	QListWidgetItem* itemNew = new QListWidgetItem();
	ui.lwChatList->insertItem(0, itemNew);
	ui.lwChatList->setItemWidget(itemNew, chatItemNew);

	ui.lwChatList->setCurrentRow(0);
}

void STMain::initContactAddNew()
{
	m_contactAddNew = new STContactAddNew(m_xmppClient);
	ui.widContactAddNew->layout()->addWidget(m_contactAddNew);
}

void STMain::initContactData()
{
	STContactItem* contactItem;

	QList<UserInfo> friendList = m_xmppClient->getRoster();
	QList<UserInfo>::const_iterator it;
	for (it = friendList.constBegin(); it != friendList.constEnd(); it++)
	{
		contactItem = new STContactItem();
		contactItem->setUserInfo(*it);
		m_contactItemList.push_back(contactItem);
	}
}

void STMain::initContactList()
{
	ui.lwContactList->clear();

	QListWidgetItem* item;
	QList<STContactItem*>::Iterator it;
	for (it = m_contactItemList.begin(); it != m_contactItemList.end(); it++)
	{
		item = new QListWidgetItem();
		ui.lwContactList->addItem(item);
		ui.lwContactList->setItemWidget(item, *it);
	}
	ui.lblFriendNum->setText(QString::number(m_contactItemList.size()));
}

void STMain::initContactMainWindow()
{
	ui.widContactDetail->setVisible(false);
	ui.widContactBlank->setVisible(true);
	ui.widContactAddNew->setVisible(false);
}

void STMain::initContactWindow()
{
	initContactData();
	initContactList();
	initContactMainWindow();
}

void STMain::init()
{
	// 初始化本人信息
	UserInfo selfInfo = m_xmppClient->getSelfInfo();
	QString path = selfInfo.photoPath;
	if (!QFile::exists(path))
	{
		path = ":/SoftTerminal/images/account.png";
	}
	QImage* image = new QImage(path);
	ui.lblUserPic->setPixmap(QPixmap::fromImage(*image).scaled(45, 45));

	// 初始化聊天
	initChatWindow();

	// 初始化联系人
	initContactWindow();
}

void STMain::destroy()
{
	// 回收资源
	qDeleteAll(m_chatItemList);
	m_chatItemList.clear();

	qDeleteAll(m_chatDetailList);
	m_chatDetailList.clear();

	qDeleteAll(m_contactItemList);
	m_contactItemList.clear();
}

void STMain::on_lwChatList_itemClicked()
{
	QListWidgetItem* item;
	item = ui.lwChatList->currentItem();
	QWidget* widget = ui.lwChatList->itemWidget(item);
	STChatItem* chatItem = (STChatItem*)widget;

	UserInfo userInfo = chatItem->getUserInfo();
	switchChatDetail(userInfo.jid);

	ui.swChatDetail->setVisible(true);
	ui.widChatBlank->setVisible(false);
}

void STMain::on_lwContactList_itemClicked()
{
	if (ui.swMain->currentIndex() == 0)
	{
		return;
	}
	QListWidgetItem* item;
	item = ui.lwContactList->currentItem();
	QWidget* widget = ui.lwContactList->itemWidget(item);
	STContactItem* contactItem = (STContactItem*)widget;

	UserInfo userInfo = contactItem->getUserInfo();
	m_contactDetail->setContactDetail(userInfo);

	ui.widContactDetail->setVisible(true);
	ui.widContactBlank->setVisible(false);
	ui.widContactAddNew->setVisible(false);
	ui.widTitle->setStyleSheet("QWidget{border:0px;background-color:#434555;}");
}

void STMain::switchChatItem(QString jid)
{
	int selectIndex = 0;
	QList<STChatItem*>::Iterator it;
	for (it = m_chatItemList.begin(); it != m_chatItemList.end(); it++, selectIndex++)
	{
		if ((*it)->getUserInfo().jid == jid)
		{
			break;
		}
	}
	ui.lwChatList->setCurrentRow(selectIndex);
}

void STMain::switchChatDetail(QString jid)
{
	int index = 0;
	QList<STChatDetail*>::Iterator it;
	for (it = m_chatDetailList.begin(); it != m_chatDetailList.end(); it++, index++)
	{
		if ((*it)->getUserInfo().jid == jid)
		{
			ui.swChatDetail->setCurrentIndex(index);
			ui.lblChatTitle->setText((*it)->getUserInfo().userName);
		}
	}
}

void STMain::switchChatWindow(QString jid)
{
	// 文件不存在，创建新对话
	STRecordManager* recordManager = new STRecordManager(jid);
	if (!recordManager->isRecordExist())
	{
		newChat(jid);
	}

	switchChatItem(jid);
	switchChatDetail(jid);

	ui.swChatDetail->setVisible(true);
	ui.widChatBlank->setVisible(false);

	on_pbChat_clicked();
}

void STMain::newChat(QString jid)
{
	// 文件不存在，创建文件
	/*STRecordManager* recordManager = new STRecordManager(jid);
	RecordItem recordItem;
	recordManager->writeRecordItem(recordItem);*/

	QList<UserInfo> friendList = m_xmppClient->getRoster();
	QList<UserInfo>::const_iterator friendIt;

	STChatItem* chatItem;
	STChatDetail* chatDetail;
	QListWidgetItem* item;
	QList<STChatItem*>::Iterator it;

	for (friendIt = friendList.constBegin(); friendIt != friendList.constEnd(); friendIt++)
	{
		if (friendIt->jid == jid)
		{
			chatItem = new STChatItem();
			chatItem->setUserInfo(*friendIt);
			m_chatItemList.push_front(chatItem);

			chatDetail = new STChatDetail(m_xmppClient, this);
			chatDetail->setChatDetail(*friendIt);
			connect(chatDetail, SIGNAL(changeChatListOrder(QString)), this, SLOT(reorderChatList(QString)));
			m_chatDetailList.push_back(chatDetail);

			ui.swChatDetail->addWidget(chatDetail);

			item = new QListWidgetItem();
			ui.lwChatList->insertItem(0, item);
			ui.lwChatList->setItemWidget(item, chatItem);
			break;
		}
	}
}

void STMain::deleteChat(QString jid)
{
	ui.swChatDetail->setVisible(false);
	ui.widChatBlank->setVisible(true);
	ui.lblChatTitle->clear();

	QList<STChatItem*>::Iterator itemIt;
	int index = 0;
	for (itemIt = m_chatItemList.begin(); itemIt != m_chatItemList.end(); itemIt++, index++)
	{
		if ((*itemIt)->getUserInfo().jid == jid)
		{
			QListWidgetItem* item = ui.lwChatList->item(index);
			ui.lwChatList->removeItemWidget(item);
			m_chatItemList.removeOne(*itemIt);
			break;
		}
	}

	QList<STChatDetail*>::Iterator detailIt;
	for (detailIt = m_chatDetailList.begin(); detailIt != m_chatDetailList.end(); detailIt++)
	{
		if ((*detailIt)->getUserInfo().jid == jid)
		{
			ui.swChatDetail->removeWidget(*detailIt);
			m_chatDetailList.removeOne(*detailIt);
			break;
		}
	}

	STRecordManager* recordManager = new STRecordManager(jid);
	recordManager->removeRecord();
}

void STMain::updateSelfPic(QString picPath)
{
	QImage* image = new QImage(picPath);
	ui.lblUserPic->setPixmap(QPixmap::fromImage(*image).scaled(45, 45));

	QList<STChatDetail*>::Iterator it;
	for (it = m_chatDetailList.begin(); it != m_chatDetailList.end(); it++)
	{
		(*it)->updateSelfPic(picPath);
	}
}

void STMain::confirmExit()
{
	m_confirmMode = "exit";
	m_confirm->setText(QStringLiteral("您是否确定退出系统？"));
	int parentX = geometry().x();
	int parentY = geometry().y();
	int parentWidth = geometry().width();
	int parentHeight = geometry().height();
	m_confirm->move(QPoint(parentX + (parentWidth - m_confirm->width()) / 2,
		parentY + (parentHeight - m_confirm->height()) / 2));
	m_confirm->exec();
}

void STMain::confirmRelogin()
{
	m_confirmMode = "relogin";
	m_confirm->setText(QStringLiteral("您是否确定切换账号？"));
	int parentX = geometry().x();
	int parentY = geometry().y();
	int parentWidth = geometry().width();
	int parentHeight = geometry().height();
	m_confirm->move(QPoint(parentX + (parentWidth - m_confirm->width()) / 2,
		parentY + (parentHeight - m_confirm->height()) / 2));
	m_confirm->exec();
}

void STMain::showSettingWindow()
{
	STSetting* settingWindow = new STSetting(this);
	int parentX = geometry().x();
	int parentY = geometry().y();
	int parentWidth = geometry().width();
	int parentHeight = geometry().height();
	settingWindow->move(QPoint(parentX + (parentWidth - settingWindow->width()) / 2,
		parentY + (parentHeight - settingWindow->height()) / 2));
	settingWindow->exec();
}

void STMain::handleConfirmOK()
{
	if (m_confirmMode == "exit")
	{
		close();
	}
	else if (m_confirmMode == "relogin")
	{
		Q_EMIT changeLoginWindow();
	}
}

void STMain::on_lwContactList_itemDoubleClicked()
{
	QListWidgetItem* item;
	item = ui.lwContactList->currentItem();
	QWidget* widget = ui.lwContactList->itemWidget(item);
	STContactItem* contactItem = (STContactItem*)widget;

	UserInfo userInfo = contactItem->getUserInfo();

	switchChatWindow(userInfo.jid);
}

void STMain::on_pbChat_clicked()
{
	ui.swMain->setCurrentIndex(0);
	ui.widSearch->setVisible(true);
	ui.pbChat->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/chat_on.png);}");
	ui.pbContact->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/contact.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/contact_focus.png);}");
	ui.pbGroup->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/group.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/group_focus.png);}");
	ui.pbCloud->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/cloud.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/cloud_focus.png);}");
	ui.widTitle->setStyleSheet("QWidget#widTitle{border-bottom:1px solid #e3e3e3;"
		"border-top:1px solid #e3e3e3;border-right:1px solid #e3e3e3;background-color:#ffffff;}");
	if (ui.lwChatList->selectedItems().size() > 0)
	{
		ui.lblChatTitle->setText(((STChatItem*)
			(ui.lwChatList->itemWidget(ui.lwChatList->currentItem())))->getUserInfo().userName);
	}
	else
	{
		ui.lblChatTitle->clear();
	}
}

void STMain::on_pbContact_clicked()
{
	ui.swMain->setCurrentIndex(1);
	ui.widSearch->setVisible(true);
	ui.pbChat->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/chat.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/chat_focus.png);}");
	ui.pbContact->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/contact_on.png);}");
	ui.pbGroup->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/group.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/group_focus.png);}");
	ui.pbCloud->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/cloud.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/cloud_focus.png);}");
	if (ui.lwContactList->selectedItems().size() > 0 && !ui.widContactAddNew->isVisible())
	{
		ui.widTitle->setStyleSheet("QWidget{border:0px;background-color:#434555;}");
	}
	ui.lblChatTitle->clear();
}

void STMain::on_pbGroup_clicked()
{
	ui.swMain->setCurrentIndex(3);
	ui.widSearch->setVisible(true);
	ui.pbChat->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/chat.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/chat_focus.png);}");
	ui.pbContact->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/contact.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/contact_focus.png);}");
	ui.pbGroup->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/group_on.png);}");
	ui.pbCloud->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/cloud.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/cloud_focus.png);}");
	ui.lblChatTitle->clear();
}

void STMain::on_pbCloud_clicked()
{
	ui.swMain->setCurrentIndex(4);
	ui.widSearch->setVisible(false);
	ui.widTitle->setStyleSheet("QWidget#widTitle{border-bottom:1px solid #e3e3e3;"
		"border-top:1px solid #e3e3e3;border-right:1px solid #e3e3e3;background-color:#ffffff;}");
	ui.pbChat->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/chat.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/chat_focus.png);}");
	ui.pbContact->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/contact.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/contact_focus.png);}");
	ui.pbGroup->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/group.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/group_focus.png);}");
	ui.pbCloud->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/cloud_on.png);}");
	ui.lblChatTitle->setText(QStringLiteral("我的云盘"));
	m_cloudFileManager->initCloudFileView();
}

void STMain::on_pbAddContact_clicked()
{
	ui.widContactBlank->setVisible(false);
	ui.widContactDetail->setVisible(false);
	ui.widContactAddNew->setVisible(true);

	m_contactAddNew->initAddNewWindow();

	on_pbContact_clicked();
	ui.widTitle->setStyleSheet("QWidget#widTitle{border-bottom:1px solid #e3e3e3;"
		"border-top:1px solid #e3e3e3;border-right:1px solid #e3e3e3;background-color:#ffffff;}");
}

void STMain::on_pbMessage_clicked()
{
}

void STMain::on_pbSetting_clicked()
{
	m_menu->hide();
	int x = geometry().x() + 15;
	int y = geometry().y() + geometry().height() - 45 - m_menu->height();
	m_menu->move(QPoint(x, y));
	m_menu->show();
}

void STMain::on_pbMinimum_clicked()
{
	this->window()->showMinimized();
}

void STMain::on_pbMaximum_clicked()
{
	ui.pbNormal->setVisible(true);
	ui.pbNormal->setAttribute(Qt::WA_UnderMouse, false);
	ui.pbMaximum->setVisible(false);
	this->window()->showMaximized();
}

void STMain::on_pbNormal_clicked()
{
	ui.pbNormal->setVisible(false);
	ui.pbMaximum->setVisible(true);
	ui.pbMaximum->setAttribute(Qt::WA_UnderMouse, false);
	this->window()->showNormal();
}

void STMain::on_pbClose_clicked()
{
	confirmExit();
}

void STMain::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}
}

void STMain::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint pos = this->pos() + movePoint;
		m_startMovePos = event->globalPos();
		this->move(pos.x(), pos.y());
	}
}

void STMain::mouseReleaseEvent(QMouseEvent* event)
{
	m_isPressed = false;
}

bool STMain::eventFilter(QObject* obj, QEvent* e)
{
	if (e->type() == QEvent::MouseButtonPress)
	{
		if (ui.lblDirect == obj || ui.lblFriend == obj
			|| ui.widContactTitle == obj || ui.lblFriendNum == obj)
		{
			bool visible = ui.lwContactList->isVisible();
			visible = !visible;
			ui.lwContactList->setVisible(visible);
			if (visible)
			{
				ui.lblDirect->setStyleSheet("border-image:url(:/SoftTerminal/images/down.png);");
			}
			else
			{
				ui.lblDirect->setStyleSheet("border-image:url(:/SoftTerminal/images/right.png);");
			}
			return true;
		}
		else if (ui.lblUserPic == obj)
		{
			m_personalInfo->initPersonalInfo();
			ui.widSearch->setVisible(false);
			ui.swMain->setCurrentIndex(2);
			ui.lblChatTitle->clear();
			ui.widTitle->setStyleSheet("QWidget#widTitle{border-bottom:1px solid #e3e3e3;"
				"border-top:1px solid #e3e3e3;border-right:1px solid #e3e3e3;background-color:#ffffff;}");
		}
	}
	else if (e->type() == QEvent::MouseButtonDblClick && ui.widTitle == obj)
	{
		if (ui.pbNormal->isVisible())
		{
			on_pbNormal_clicked();
		}
		else
		{
			on_pbMaximum_clicked();
		}
	}
	return false;
}
