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
	searchBtn->setStyleSheet("QPushButton{border-image:url(:/SoftTerminal/images/search.png);"
		"background:transparent;}");
	mainLayout->addWidget(searchBtn);

	mainLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	m_clearBtn = new QPushButton;
	m_clearBtn->setFixedSize(13, 13);
	m_clearBtn->setCursor(Qt::PointingHandCursor);
	m_clearBtn->setStyleSheet("QPushButton{border-image:url(:/SoftTerminal/images/clear.png);"
		"background:transparent;}");
	mainLayout->addWidget(m_clearBtn);

	mainLayout->setContentsMargins(8, 0, 8, 0);

	ui.leContactSearch->setTextMargins(13 + 12, 0, 13 + 12, 0);
	ui.leContactSearch->setContentsMargins(0, 0, 0, 0);
	ui.leContactSearch->setLayout(mainLayout);
	connect(ui.leContactSearch, SIGNAL(textChanged(const QString)), this, SLOT(onTextChanged()));
	connect(m_clearBtn, SIGNAL(clicked()), this, SLOT(clearSearchInput()));
	m_clearBtn->setVisible(false);

	// 初始化左侧工具栏
	ui.pbChat->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/chat_on.png);}");

	m_menu = new STMenu(this);
	connect(m_menu, SIGNAL(confirmExit()), this, SLOT(confirmExit()));
	connect(m_menu, SIGNAL(confirmRelogin()), this, SLOT(confirmRelogin()));
	connect(m_menu, SIGNAL(showSettingWindow()), this, SLOT(showSettingWindow()));
	
	m_menu->hide();
	m_confirm = new STConfirm(this);
	connect(m_confirm, SIGNAL(confirmOK()), this, SLOT(handleConfirmOK()));


	connect(m_xmppClient, SIGNAL(subscriptionRequest(QString)), this, SLOT(showMessageWarn()));
	connect(m_xmppClient, SIGNAL(refreshContactSignal()), this, SLOT(refreshContact()));

	m_messageCenterWindow = new STMessageCenter(m_xmppClient, this);
	if (m_xmppClient->getSubscriptionRequests().size() > 0)
	{
		showMessageWarn();
	}

	// 初始化右侧窗口
	initContactAddNew();
	initGroupAddNew();

	m_contactDetail = new STContactDetail(this);
	ui.widContactDetail->layout()->addWidget(m_contactDetail);
	connect(m_contactDetail, SIGNAL(openChatDetail(QString)), this, SLOT(switchChatWindow(QString)));
	connect(m_contactDetail, SIGNAL(deleteFriend(QString)), this, SLOT(deleteFriend(QString)));

	m_groupDetail = new STGroupDetail(m_xmppClient, this);
	connect(m_groupDetail, SIGNAL(openChatDetail(QString)), this, SLOT(switchChatWindow(QString)));
	connect(m_groupDetail, SIGNAL(refreshGroupSignal(QString)), this, SLOT(refreshGroup(QString)));
	ui.widGroupDetail->layout()->addWidget(m_groupDetail);

	ui.lblDirect->installEventFilter(this);
	ui.lblFriend->installEventFilter(this);
	ui.widContactTitle->installEventFilter(this);
	ui.lblFriendNum->installEventFilter(this);
	ui.widTitle->installEventFilter(this);
	ui.lblUserPic->installEventFilter(this);
	ui.leContactSearch->installEventFilter(this);

	m_personalInfo = new STPersonalInfo(m_xmppClient);
	connect(m_personalInfo, SIGNAL(updateSelfPic(QString)), this, SLOT(updateSelfPic(QString)));
	ui.pgPersonalInfo->layout()->addWidget(m_personalInfo);

	m_cloudFileManager = new STCloudFileManager();
	ui.pgCloudFileWindow->layout()->addWidget(m_cloudFileManager);

	setPageIndex(0);
}

STMain::~STMain()
{
	delete m_contactDetail;
	delete m_groupDetail;
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

	QList<XmppGroup*> groupList = m_xmppClient->getGroups();
	QList<XmppGroup*>::const_iterator groupIt;

	STChatItem* chatItem;
	STChatDetail* chatDetail;
	QList<QString>::Iterator fileIt;
	UserInfo groupInfo;
	groupInfo.photoPath = ":/SoftTerminal/images/group_icon.png";
	// 过滤
	for (fileIt = files.begin(); fileIt != files.end(); fileIt++)
	{
		for (friendIt = friendList.constBegin(); friendIt != friendList.constEnd(); friendIt++)
		{
			if (fileIt->startsWith(friendIt->jid))
			{
				chatItem = new STChatItem();
				chatItem->setUserInfo(*friendIt);
				m_chatItemList.push_back(chatItem);

				chatDetail = new STChatDetail(m_xmppClient, this);
				chatDetail->setChatDetail(*friendIt);
				connect(chatDetail, SIGNAL(changeChatListOrder(QString)), this, SLOT(reorderChatList(QString)));
				m_chatDetailList.push_back(chatDetail);

				ui.swChatDetail->addWidget(chatDetail);
				break;
			}
		}

		for (groupIt = groupList.constBegin(); groupIt != groupList.constEnd(); groupIt++)
		{
			if (fileIt->startsWith((*groupIt)->getGroupInfo().id))
			{
				groupInfo.jid = (*groupIt)->getGroupInfo().id;
				groupInfo.userName = (*groupIt)->getGroupInfo().name;

				chatItem = new STChatItem();
				chatItem->setUserInfo(groupInfo);
				m_chatItemList.push_back(chatItem);

				chatDetail = new STChatDetail(m_xmppClient, this);
				chatDetail->setChatDetail(groupInfo, *groupIt);
				connect(chatDetail, SIGNAL(changeChatListOrder(QString)), this, SLOT(reorderChatList(QString)));
				m_chatDetailList.push_back(chatDetail);

				ui.swChatDetail->addWidget(chatDetail);
				break;
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

void STMain::loadChatWindow()
{
	initChatData();
	initChatList();
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
	m_contactItemList.clear();
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

void STMain::loadContactWindow()
{
	initContactData();
	initContactList();
}

void STMain::refreshContact()
{
	loadContactWindow();
	//initContactMainWindow();
}

void STMain::initGroupAddNew()
{
	m_groupAddNew = new STGroupAddNew(m_xmppClient);
	connect(m_groupAddNew, SIGNAL(refreshGroupSignal(QString)), this, SLOT(refreshGroup(QString)));
	ui.widGroupAddNew->layout()->addWidget(m_groupAddNew);
}

void STMain::initGroupData()
{
	m_groupItemList.clear();
	STGroupItem* groupItem;

	QList<XmppGroup*> groupList = m_xmppClient->getGroups();
	QList<XmppGroup*>::const_iterator it;
	for (it = groupList.constBegin(); it != groupList.constEnd(); it++)
	{
		groupItem = new STGroupItem(m_xmppClient, *it);
		m_groupItemList.push_back(groupItem);
	}
}

void STMain::initGroupList()
{
	ui.lwGroupList->clear();

	QListWidgetItem* item;
	QList<STGroupItem*>::Iterator it;
	for (it = m_groupItemList.begin(); it != m_groupItemList.end(); it++)
	{
		item = new QListWidgetItem();
		ui.lwGroupList->addItem(item);
		ui.lwGroupList->setItemWidget(item, *it);
	}
	ui.lblGroupNum->setText(QString::number(m_groupItemList.size()));
}

void STMain::initGroupMainWindow()
{
	ui.widGroupDetail->setVisible(false);
	ui.widGroupBlank->setVisible(true);
	ui.widGroupAddNew->setVisible(false);
}

void STMain::loadGroupWindow()
{
	initGroupData();
	initGroupList();
}

void STMain::refreshGroup(QString id)
{
	loadGroupWindow();

	QListWidgetItem* item;
	QWidget* widget;
	STGroupItem* groupItem;
	for (int i = 0; i < ui.lwGroupList->count(); i++)
	{
		item = ui.lwGroupList->item(i);
		widget = ui.lwGroupList->itemWidget(item);
		groupItem = (STGroupItem*)widget;

		if (groupItem->getGroupInfo().id == id)
		{
			ui.lwGroupList->setCurrentItem(item);
			return;
		}
	}
}

void STMain::updateOthersMessage(QString jid)
{
	STChatItem* chatItem;
	QList<STChatItem*>::Iterator it;
	for (it = m_chatItemList.begin(); it != m_chatItemList.end(); it++)
	{
		if ((*it)->getUserInfo().jid == jid)
		{
			if (m_currentChatJid != jid)
			{
				(*it)->updateUnreadNum();
			}
			(*it)->updateMessage();
			break;
		}
	}
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
	initChatMainWindow();
	// 初始化联系人
	initContactMainWindow();
	// 初始化群组
	initGroupMainWindow();

	connect(this, SIGNAL(loadChatWindowSignal()), this, SLOT(loadChatWindow()));
	connect(this, SIGNAL(loadContactWindowSignal()), this, SLOT(loadContactWindow()));
	connect(this, SIGNAL(loadGroupWindowSignal()), this, SLOT(loadGroupWindow()));

	pthread_create(&m_tidLoad, NULL, loadProc, this);
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

void STMain::loadInfo()
{
	Q_EMIT loadChatWindowSignal();
	Q_EMIT loadContactWindowSignal();
	Q_EMIT loadGroupWindowSignal();
}

void* STMain::loadProc(void* args)
{
	STMain* main = (STMain*)args;
	main->loadInfo();
	return NULL;
}

void STMain::showMessageWarn()
{
	if (!m_messageCenterWindow->isVisible())
	{
		ui.pbMessage->setStyleSheet("QPushButton {border-image: url(:/SoftTerminal/images/message_warn.png);}"
			"QPushButton:hover:!pressed {border-image: url(:/SoftTerminal/images/message_warn_focus.png);}");
	}
}

void STMain::on_lwChatList_itemClicked()
{
	QListWidgetItem* item;
	item = ui.lwChatList->currentItem();
	QWidget* widget = ui.lwChatList->itemWidget(item);
	STChatItem* chatItem = (STChatItem*)widget;
	chatItem->clearUnreadNum();

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
}

void STMain::on_lwGroupList_itemClicked()
{
	if (ui.swMain->currentIndex() == 0)
	{
		return;
	}
	QListWidgetItem* item;
	item = ui.lwGroupList->currentItem();
	QWidget* widget = ui.lwGroupList->itemWidget(item);
	STGroupItem* groupItem = (STGroupItem*)widget;

	XmppGroup* group = groupItem->getGroup();
	GroupInfo groupInfo = groupItem->getGroupInfo();
	UserInfo ownerInfo = groupItem->getOwnerInfo();
	QMap<QString, UserInfo> membersInfo = groupItem->getMembersInfo();
	m_groupDetail->setGroupDetail(group, groupInfo, ownerInfo, membersInfo);

	ui.widGroupDetail->setVisible(true);
	ui.widGroupBlank->setVisible(false);
	ui.widGroupAddNew->setVisible(false);
}

void STMain::switchChatItem(QString jid)
{
	int selectIndex = 0;
	QList<STChatItem*>::Iterator it;
	for (it = m_chatItemList.begin(); it != m_chatItemList.end(); it++, selectIndex++)
	{
		if ((*it)->getUserInfo().jid == jid)
		{
			(*it)->clearUnreadNum();
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
			m_currentChatJid = jid;
			break;
		}
	}
}

void STMain::switchChatWindow(QString jid)
{
	createChat(jid);
	switchChatItem(jid);
	switchChatDetail(jid);

	ui.swChatDetail->setVisible(true);
	ui.widChatBlank->setVisible(false);

	on_pbChat_clicked();
}

void STMain::deleteFriend(QString jid)
{
	m_xmppClient->unsubscribeOther(jid);
}

void STMain::createChat(QString jid)
{
	// 文件不存在，创建文件
	STRecordManager* recordManager = new STRecordManager(jid);
	if (!recordManager->isRecordExist())
	{
		RecordItem recordItem;
		recordManager->writeRecordItem(recordItem);
	}

	QList<STChatItem*>::Iterator it;
	for (it = m_chatItemList.begin(); it != m_chatItemList.end(); it++)
	{
		if ((*it)->getUserInfo().jid == jid)
		{
			return;
		}
	}

	STChatItem* chatItem;
	STChatDetail* chatDetail;
	QListWidgetItem* item;

	QList<UserInfo> friendList = m_xmppClient->getRoster();
	QList<UserInfo>::const_iterator friendIt;
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

	QList<XmppGroup*> groupList = m_xmppClient->getGroups();
	QList<XmppGroup*>::const_iterator groupIt;
	UserInfo groupInfo;
	groupInfo.photoPath = ":/SoftTerminal/images/group_icon.png";
	for (groupIt = groupList.constBegin(); groupIt != groupList.constEnd(); groupIt++)
	{
		if ((*groupIt)->getGroupInfo().id == jid)
		{
			groupInfo.jid = jid;
			groupInfo.userName = (*groupIt)->getGroupInfo().name;

			chatItem = new STChatItem();
			chatItem->setUserInfo(groupInfo);
			m_chatItemList.push_front(chatItem);

			chatDetail = new STChatDetail(m_xmppClient, this);
			chatDetail->setChatDetail(groupInfo, *groupIt);
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
		Q_EMIT closeMain();
		close();
	}
	else if (m_confirmMode == "relogin")
	{
		Q_EMIT changeLoginWindow();
	}
}

void STMain::setPageIndex(int index)
{
	m_currentPageIndex = index;
	ui.swMain->setCurrentIndex(m_currentPageIndex);
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

void STMain::on_lwGroupList_itemDoubleClicked()
{
	QListWidgetItem* item;
	item = ui.lwGroupList->currentItem();
	QWidget* widget = ui.lwGroupList->itemWidget(item);
	STGroupItem* groupItem = (STGroupItem*)widget;

	switchChatWindow(groupItem->getGroupInfo().id);
}

void STMain::on_pbChat_clicked()
{
	setPageIndex(0);
	ui.widSearch->setVisible(true);
	ui.pbChat->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/chat_on.png);}");
	ui.pbContact->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/contact.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/contact_focus.png);}");
	ui.pbGroup->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/group.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/group_focus.png);}");
	ui.pbCloud->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/cloud.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/cloud_focus.png);}");
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
	setPageIndex(1);
	ui.widSearch->setVisible(true);
	ui.pbChat->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/chat.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/chat_focus.png);}");
	ui.pbContact->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/contact_on.png);}");
	ui.pbGroup->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/group.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/group_focus.png);}");
	ui.pbCloud->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/cloud.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/cloud_focus.png);}");
	ui.lblChatTitle->clear();
}

void STMain::on_pbGroup_clicked()
{
	setPageIndex(3);
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
	setPageIndex(4);
	ui.widSearch->setVisible(false);
	ui.pbChat->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/chat.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/chat_focus.png);}");
	ui.pbContact->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/contact.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/contact_focus.png);}");
	ui.pbGroup->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/group.png);}"
		"QPushButton:hover:!pressed{border-image:url(:/SoftTerminal/images/group_focus.png);}");
	ui.pbCloud->setStyleSheet("QPushButton{border-image: url(:/SoftTerminal/images/cloud_on.png);}");
	ui.lblChatTitle->clear();
	m_cloudFileManager->initCloudFileView();
}

void STMain::on_pbAddContact_clicked()
{
	ui.widContactBlank->setVisible(false);
	ui.widContactDetail->setVisible(false);
	ui.widContactAddNew->setVisible(true);

	m_contactAddNew->initAddNewWindow();
}

void STMain::on_pbNewGroup_clicked()
{
	ui.widGroupBlank->setVisible(false);
	ui.widGroupDetail->setVisible(false);
	ui.widGroupAddNew->setVisible(true);

	m_groupAddNew->initAddNewWindow();
}

void STMain::on_pbMessage_clicked()
{
	ui.pbMessage->setStyleSheet("QPushButton {border-image: url(:/SoftTerminal/images/message.png);}"
		"QPushButton:hover:!pressed {border-image: url(:/SoftTerminal/images/message_focus.png);}");
	int parentX = geometry().x();
	int parentY = geometry().y();
	int parentWidth = geometry().width();
	int parentHeight = geometry().height();
	m_messageCenterWindow->move(QPoint(parentX + (parentWidth - m_messageCenterWindow->width()) / 2,
		parentY + (parentHeight - m_messageCenterWindow->height()) / 2));
	m_messageCenterWindow->exec();
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
			setPageIndex(2);
			ui.lblChatTitle->clear();
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
	else if (ui.leContactSearch == obj)
	{
		if (e->type() == QEvent::FocusIn)
		{
			m_clearBtn->setVisible(true);
			refreshSearchList();
			//ui.swMain->setCurrentIndex(5);
		}
		else if (e->type() == QEvent::FocusOut)
		{
			clearSearchInput();
		}
	}
	return false;
}

void STMain::refreshSearchData()
{
	m_searchItemList.clear();

	STContactItem* contactItem;

	QList<UserInfo> friendList = m_xmppClient->getRoster();
	QList<UserInfo>::const_iterator it;
	for (it = friendList.constBegin(); it != friendList.constEnd(); it++)
	{
		contactItem = new STContactItem();
		contactItem->setUserInfo(*it);
		m_searchItemList.push_back(contactItem);
	}
}

void STMain::refreshSearchList()
{
	ui.lwSearchList->clear();
	QString text = ui.leContactSearch->text();
	if (text.isEmpty())
	{
		ui.lblSearch->setText(QStringLiteral("请输入搜索条件"));
		ui.lblSearchNum->clear();
		return;
	}
	refreshSearchData();
	int count = 0;
	QListWidgetItem* item;
	QList<STContactItem*>::Iterator it;
	for (it = m_searchItemList.begin(); it != m_searchItemList.end(); it++)
	{
		if (!(*it)->getUserInfo().userName.contains(text))
		{
			continue;
		}
		item = new QListWidgetItem();
		ui.lwSearchList->addItem(item);
		ui.lwSearchList->setItemWidget(item, *it);
		count++;
	}
	ui.lblSearch->setText(QStringLiteral("符合搜索条件的好友"));
	ui.lblSearchNum->setText(QString::number(count));
}

void STMain::onTextChanged()
{
	refreshSearchList();
}

void STMain::clearSearchInput()
{
	ui.swMain->setCurrentIndex(m_currentPageIndex);
	ui.leContactSearch->clear();
	m_clearBtn->setVisible(false);
}

void STMain::on_lwSearchList_itemDoubleClicked()
{
	QListWidgetItem* item;
	item = ui.lwSearchList->currentItem();
	QWidget* widget = ui.lwSearchList->itemWidget(item);
	STContactItem* contactItem = (STContactItem*)widget;

	UserInfo userInfo = contactItem->getUserInfo();

	switchChatWindow(userInfo.jid);
}
