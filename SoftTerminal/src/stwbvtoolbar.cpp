#include "stwbvtoolbar.h"

using namespace tahiti;

// ------------------------ STWBRaiseHandPanel ------------------------
STWBRaiseHandPanel::STWBRaiseHandPanel(QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::SubWindow);
	m_isRaised = false;
}

STWBRaiseHandPanel::~STWBRaiseHandPanel()
{

}

void STWBRaiseHandPanel::on_pbRaiseHand_clicked()
{
	m_isRaised = !m_isRaised;
	if (m_isRaised)
	{
		ui.pbRaiseHand->setStyleSheet("border-image: url(:/SoftTerminal/images/hand_on.png);");
	}
	else
	{
		ui.pbRaiseHand->setStyleSheet("QPushButton{border-radius: 3px;color: rgb(255, 255, 255);"
			"border-image: url(:/SoftTerminal/images/hand.png);}"
			"QPushButton:hover:pressed{border-image: url(:/SoftTerminal/images/hand.png);}"
			"QPushButton:hover:!pressed{border-image: url(:/SoftTerminal/images/hand_focus.png);}");
	}
	Q_EMIT raiseHandSignal(m_isRaised);
}

// ------------------------ STWBPenStylePanel ------------------------
STWBPenStylePanel::STWBPenStylePanel(QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::SubWindow);
	ui.lblWhite->installEventFilter(this);
	ui.lblLightgray->installEventFilter(this);
	ui.lblGray->installEventFilter(this);
	ui.lblBlack->installEventFilter(this);
	ui.lblRed->installEventFilter(this);
	ui.lblOrange->installEventFilter(this);
	ui.lblYellow->installEventFilter(this);
	ui.lblGreen->installEventFilter(this);
	ui.lblSkyblue->installEventFilter(this);
	ui.lblBlue->installEventFilter(this);
	ui.lblPurple->installEventFilter(this);
	ui.lblPink->installEventFilter(this);

}

STWBPenStylePanel::~STWBPenStylePanel()
{

}

void STWBPenStylePanel::init()
{
	on_pb2_clicked();
	Q_EMIT updatePenColor("#ff0000");
}

void STWBPenStylePanel::on_pb1_clicked()
{
	ui.pb1->setStyleSheet("border-image: url(:/SoftTerminal/images/1_focus.png);");
	ui.pb2->setStyleSheet("border-image: url(:/SoftTerminal/images/2.png);");
	ui.pb3->setStyleSheet("border-image: url(:/SoftTerminal/images/3.png);");
	ui.pb4->setStyleSheet("border-image: url(:/SoftTerminal/images/4.png);");
	m_currentSelect = 1;
	Q_EMIT updatePenThickness(2);
}

void STWBPenStylePanel::on_pb2_clicked()
{
	ui.pb1->setStyleSheet("border-image: url(:/SoftTerminal/images/1.png);");
	ui.pb2->setStyleSheet("border-image: url(:/SoftTerminal/images/2_focus.png);");
	ui.pb3->setStyleSheet("border-image: url(:/SoftTerminal/images/3.png);");
	ui.pb4->setStyleSheet("border-image: url(:/SoftTerminal/images/4.png);");
	m_currentSelect = 2;
	Q_EMIT updatePenThickness(6);
}

void STWBPenStylePanel::on_pb3_clicked()
{
	ui.pb1->setStyleSheet("border-image: url(:/SoftTerminal/images/1.png);");
	ui.pb2->setStyleSheet("border-image: url(:/SoftTerminal/images/2.png);");
	ui.pb3->setStyleSheet("border-image: url(:/SoftTerminal/images/3_focus.png);");
	ui.pb4->setStyleSheet("border-image: url(:/SoftTerminal/images/4.png);");
	m_currentSelect = 3;
	Q_EMIT updatePenThickness(10);
}

void STWBPenStylePanel::on_pb4_clicked()
{
	ui.pb1->setStyleSheet("border-image: url(:/SoftTerminal/images/1.png);");
	ui.pb2->setStyleSheet("border-image: url(:/SoftTerminal/images/2.png);");
	ui.pb3->setStyleSheet("border-image: url(:/SoftTerminal/images/3.png);");
	ui.pb4->setStyleSheet("border-image: url(:/SoftTerminal/images/4_focus.png);");
	m_currentSelect = 4;
	Q_EMIT updatePenThickness(16);
}

bool STWBPenStylePanel::eventFilter(QObject* watched, QEvent* e)
{
	// 鼠标进入或离开关闭按钮时设置为不同状态的图标
	if (e->type() == QEvent::MouseButtonPress)
	{
		if (watched == ui.lblWhite)
		{
			Q_EMIT updatePenColor("#ffffff");
		}
		else if (watched == ui.lblLightgray)
		{
			Q_EMIT updatePenColor("#c3c3c3");
		}
		else if (watched == ui.lblGray)
		{
			Q_EMIT updatePenColor("#7e7e7e");
		}
		else if (watched == ui.lblBlack)
		{
			Q_EMIT updatePenColor("#000000");
		}
		else if (watched == ui.lblRed)
		{
			Q_EMIT updatePenColor("#ff0000");
		}
		else if (watched == ui.lblOrange)
		{
			Q_EMIT updatePenColor("#ffaa00");
		}
		else if (watched == ui.lblYellow)
		{
			Q_EMIT updatePenColor("#ffff7f");
		}
		else if (watched == ui.lblGreen)
		{
			Q_EMIT updatePenColor("#00aa00");
		}
		else if (watched == ui.lblSkyblue)
		{
			Q_EMIT updatePenColor("#00aaff");
		}
		else if (watched == ui.lblBlue)
		{
			Q_EMIT updatePenColor("#0000ff");
		}
		else if (watched == ui.lblPurple)
		{
			Q_EMIT updatePenColor("#aaaaff");
		}
		else if (watched == ui.lblPink)
		{
			Q_EMIT updatePenColor("#ffaaff");
		}
	}

	return QWidget::eventFilter(watched, e);
}

// ------------------------ STWBTextStylePanel ------------------------
STWBTextStylePanel::STWBTextStylePanel(QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::SubWindow);
	ui.lblWhite->installEventFilter(this);
	ui.lblLightgray->installEventFilter(this);
	ui.lblGray->installEventFilter(this);
	ui.lblBlack->installEventFilter(this);
	ui.lblRed->installEventFilter(this);
	ui.lblOrange->installEventFilter(this);
	ui.lblYellow->installEventFilter(this);
	ui.lblGreen->installEventFilter(this);
	ui.lblSkyblue->installEventFilter(this);
	ui.lblBlue->installEventFilter(this);
	ui.lblPurple->installEventFilter(this);
	ui.lblPink->installEventFilter(this);
}

STWBTextStylePanel::~STWBTextStylePanel()
{

}

void STWBTextStylePanel::init()
{
	on_pb2_clicked();
	Q_EMIT updateTextColor("#00aa00");
}

void STWBTextStylePanel::on_pb1_clicked()
{
	ui.pb1->setStyleSheet("border:0px;color:#ffffff;");
	ui.pb2->setStyleSheet("border:0px;color:#8e9096;");
	ui.pb3->setStyleSheet("border:0px;color:#8e9096;");
	ui.pb4->setStyleSheet("border:0px;color:#8e9096;");
	m_currentSelect = 1;
	Q_EMIT updateTextSize(12);
}

void STWBTextStylePanel::on_pb2_clicked()
{
	ui.pb1->setStyleSheet("border:0px;color:#8e9096;");
	ui.pb2->setStyleSheet("border:0px;color:#ffffff;");
	ui.pb3->setStyleSheet("border:0px;color:#8e9096;");
	ui.pb4->setStyleSheet("border:0px;color:#8e9096;");
	m_currentSelect = 2;
	Q_EMIT updateTextSize(16);
}

void STWBTextStylePanel::on_pb3_clicked()
{
	ui.pb1->setStyleSheet("border:0px;color:#8e9096;");
	ui.pb2->setStyleSheet("border:0px;color:#8e9096;");
	ui.pb3->setStyleSheet("border:0px;color:#ffffff;");
	ui.pb4->setStyleSheet("border:0px;color:#8e9096;");
	m_currentSelect = 3;
	Q_EMIT updateTextSize(24);
}

void STWBTextStylePanel::on_pb4_clicked()
{
	ui.pb1->setStyleSheet("border:0px;color:#8e9096;");
	ui.pb2->setStyleSheet("border:0px;color:#8e9096;");
	ui.pb3->setStyleSheet("border:0px;color:#8e9096;");
	ui.pb4->setStyleSheet("border:0px;color:#ffffff;");
	m_currentSelect = 4;
	Q_EMIT updateTextSize(36);
}

bool STWBTextStylePanel::eventFilter(QObject* watched, QEvent* e)
{
	// 鼠标进入或离开关闭按钮时设置为不同状态的图标
	if (e->type() == QEvent::MouseButtonPress)
	{
		if (watched == ui.lblWhite)
		{
			Q_EMIT updateTextColor("#ffffff");
		}
		else if (watched == ui.lblLightgray)
		{
			Q_EMIT updateTextColor("#c3c3c3");
		}
		else if (watched == ui.lblGray)
		{
			Q_EMIT updateTextColor("#7e7e7e");
		}
		else if (watched == ui.lblBlack)
		{
			Q_EMIT updateTextColor("#000000");
		}
		else if (watched == ui.lblRed)
		{
			Q_EMIT updateTextColor("#ff0000");
		}
		else if (watched == ui.lblOrange)
		{
			Q_EMIT updateTextColor("#ffaa00");
		}
		else if (watched == ui.lblYellow)
		{
			Q_EMIT updateTextColor("#ffff7f");
		}
		else if (watched == ui.lblGreen)
		{
			Q_EMIT updateTextColor("#00aa00");
		}
		else if (watched == ui.lblSkyblue)
		{
			Q_EMIT updateTextColor("#00aaff");
		}
		else if (watched == ui.lblBlue)
		{
			Q_EMIT updateTextColor("#0000ff");
		}
		else if (watched == ui.lblPurple)
		{
			Q_EMIT updateTextColor("#aaaaff");
		}
		else if (watched == ui.lblPink)
		{
			Q_EMIT updateTextColor("#ffaaff");
		}
	}

	return QWidget::eventFilter(watched, e);
}

// ------------------------ STWBVToolbar ------------------------
STWBVToolbar::STWBVToolbar(QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::SubWindow);
	setMaximumWidth(52);
	ui.pbSelect->installEventFilter(this);
	ui.pbPen->installEventFilter(this);
	ui.pbText->installEventFilter(this);
	ui.pbCloud->installEventFilter(this);
	ui.pbDelete->installEventFilter(this);
	ui.pbInvite->installEventFilter(this);
	ui.pbRoster->installEventFilter(this);
	ui.pbDeleteCourse->installEventFilter(this);

	m_currentSelect = 0;

	m_showCloudFileView = false;
	m_showInviteFriend = false;
	m_showRoster = false;

	layout()->setSizeConstraint(QLayout::SetFixedSize);
}

STWBVToolbar::~STWBVToolbar()
{

}
void STWBVToolbar::changePenShowColor(QString color)
{
	ui.lblPenColor->setStyleSheet(QString("background-color:") + color + QString(";"));
}

void STWBVToolbar::changeTextShowColor(QString color)
{
	ui.lblTextColor->setStyleSheet(QString("background-color:") + color + QString(";"));
}

void STWBVToolbar::on_pbSelect_clicked()
{
	if (m_currentSelect == 1)
	{
		ui.widSelect->setStyleSheet("");
		ui.pbSelect->setStyleSheet("border-image: url(:/SoftTerminal/images/mouse.png);");
		m_currentSelect = 0;
	}
	else
	{
		ui.widSelect->setStyleSheet("background-color:rgb(46,48,55);");
		ui.pbSelect->setStyleSheet("border-image: url(:/SoftTerminal/images/mouse_focus.png);");
		m_currentSelect = 1;
	}
	ui.widPen->setStyleSheet("");
	ui.widText->setStyleSheet("");
	ui.pbPen->setStyleSheet("border-image: url(:/SoftTerminal/images/pen.png);");
	ui.pbText->setStyleSheet("border-image: url(:/SoftTerminal/images/text.png);");
	Q_EMIT setActionMode(m_currentSelect);
}

void STWBVToolbar::on_pbPen_clicked()
{
	if (m_currentSelect == 2)
	{
		ui.widPen->setStyleSheet("");
		ui.pbPen->setStyleSheet("border-image: url(:/SoftTerminal/images/pen.png);");
		m_currentSelect = 0;
	}
	else
	{
		ui.widPen->setStyleSheet("background-color:rgb(46,48,55);");
		ui.pbPen->setStyleSheet("border-image: url(:/SoftTerminal/images/pen_focus.png);");
		m_currentSelect = 2;
	}
	ui.widSelect->setStyleSheet("");
	ui.widText->setStyleSheet("");
	ui.pbSelect->setStyleSheet("border-image: url(:/SoftTerminal/images/mouse.png);");
	ui.pbText->setStyleSheet("border-image: url(:/SoftTerminal/images/text.png);");
	Q_EMIT setActionMode(m_currentSelect);
}

void STWBVToolbar::on_pbText_clicked()
{
	if (m_currentSelect == 3)
	{
		ui.widText->setStyleSheet("");
		ui.pbText->setStyleSheet("border-image: url(:/SoftTerminal/images/text.png);");
		m_currentSelect = 0;
	}
	else
	{
		ui.widText->setStyleSheet("background-color:rgb(46,48,55);");
		ui.pbText->setStyleSheet("border-image: url(:/SoftTerminal/images/text_focus.png);");
		m_currentSelect = 3;
	}
	ui.widSelect->setStyleSheet("");
	ui.widPen->setStyleSheet("");
	ui.pbSelect->setStyleSheet("border-image: url(:/SoftTerminal/images/mouse.png);");
	ui.pbPen->setStyleSheet("border-image: url(:/SoftTerminal/images/pen.png);");
	Q_EMIT setActionMode(m_currentSelect);
}

void STWBVToolbar::on_pbDelete_clicked()
{
	Q_EMIT deleteAction();
}

void STWBVToolbar::on_pbCloud_clicked()
{
	if (m_showCloudFileView)
	{
		ui.widCloud->setStyleSheet("");
		ui.pbCloud->setStyleSheet("border-image: url(:/SoftTerminal/images/cloud.png);");

		Q_EMIT closeCloudFileViewSignal();
	}
	else
	{
		ui.widCloud->setStyleSheet("background-color:rgb(46,48,55);");
		ui.pbCloud->setStyleSheet("border-image: url(:/SoftTerminal/images/cloud_on.png);");

		Q_EMIT openCloudFileViewSignal();
	}
	m_showCloudFileView = !m_showCloudFileView;
}

void STWBVToolbar::on_pbInvite_clicked()
{
	if (m_showInviteFriend)
	{
		ui.widInvite->setStyleSheet("");
		ui.pbInvite->setStyleSheet("border-image: url(:/SoftTerminal/images/invite.png);");

		Q_EMIT closeInviteFriendSignal();
	}
	else
	{
		ui.widInvite->setStyleSheet("background-color:rgb(46,48,55);");
		ui.pbInvite->setStyleSheet("border-image: url(:/SoftTerminal/images/invite_on.png);");

		Q_EMIT openInviteFriendSignal();
	}
	m_showInviteFriend = !m_showInviteFriend;
}

void STWBVToolbar::on_pbRoster_clicked()
{
	if (m_showRoster)
	{
		ui.widRoster->setStyleSheet("");
		ui.pbRoster->setStyleSheet("border-image: url(:/SoftTerminal/images/roster.png);");

		Q_EMIT closeRosterSignal();
	}
	else
	{
		ui.widRoster->setStyleSheet("background-color:rgb(46,48,55);");
		ui.pbRoster->setStyleSheet("border-image: url(:/SoftTerminal/images/roster_on.png);");

		Q_EMIT openRosterSignal();
	}
	m_showRoster = !m_showRoster;
}

void STWBVToolbar::on_pbDeleteCourse_clicked()
{
	Q_EMIT deleteCourseSignal();
}

void STWBVToolbar::closeCloudFileView()
{
	on_pbCloud_clicked();
}

void STWBVToolbar::closeInviteFriend()
{
	on_pbInvite_clicked();
}

void STWBVToolbar::closeRoster()
{
	on_pbRoster_clicked();
}

void STWBVToolbar::toolbarModeSlot(int mode)
{
	if (mode == ToolbarMode::Video_Teacher ||
		mode == ToolbarMode::Video_Student ||
		mode == ToolbarMode::WhiteBoard_Student)
	{
		ui.widSelect->setVisible(false);
		ui.widPen->setVisible(false);
		ui.widText->setVisible(false);
		ui.widDelete->setVisible(false);
		ui.widCloud->setVisible(false);
		ui.widInvite->setVisible(false);
		ui.widDeleteCourse->setVisible(false);
		ui.line->setVisible(false);
		ui.line_2->setVisible(false);
		if (mode == ToolbarMode::Video_Teacher)
		{
			ui.widDeleteCourse->setVisible(true);
			ui.line_2->setVisible(true);
		}
	}
	else if (mode == ToolbarMode::WhiteBoard_Teacher ||
		mode == ToolbarMode::WhiteBoard_Operator)
	{
		ui.widSelect->setVisible(true);
		ui.widPen->setVisible(true);
		ui.widText->setVisible(true);
		ui.widDelete->setVisible(true);
		ui.widCloud->setVisible(true);
		ui.widInvite->setVisible(false);
		ui.widDeleteCourse->setVisible(true);
		ui.line->setVisible(true);
		ui.line_2->setVisible(true);
		if (mode == ToolbarMode::WhiteBoard_Operator)
		{
			ui.widCloud->setVisible(false);
			ui.widDeleteCourse->setVisible(false);
			ui.line_2->setVisible(false);
		}
	}
}

void STWBVToolbar::paintEvent(QPaintEvent *e)
{
	int toolbarX = parentWidget()->geometry().width() - width() - 10;
	int toolbarY = parentWidget()->geometry().height() / 2 - height() / 2;
	move(QPoint(toolbarX, toolbarY));
	QWidget::paintEvent(e);
}

void STWBVToolbar::init()
{
	m_currentSelect = 0;
	Q_EMIT setActionMode(m_currentSelect);
	ui.widSelect->setStyleSheet("");
	ui.widPen->setStyleSheet("");
	ui.widText->setStyleSheet("");

	m_showCloudFileView = false;
	ui.widCloud->setStyleSheet("");
	ui.pbCloud->setStyleSheet("border-image: url(:/SoftTerminal/images/cloud.png);");

	m_showInviteFriend = false;
	ui.widInvite->setStyleSheet("");
	ui.pbInvite->setStyleSheet("border-image: url(:/SoftTerminal/images/invite.png);");

	m_showRoster = false;
	ui.widRoster->setStyleSheet("");
	ui.pbRoster->setStyleSheet("border-image: url(:/SoftTerminal/images/roster.png);");
}

bool STWBVToolbar::eventFilter(QObject* watched, QEvent* e)
{
	// 鼠标进入或离开关闭按钮时设置为不同状态的图标
	if (watched == ui.pbSelect)
	{
		if (e->type() == QEvent::Enter)
		{
			ui.widSelect->setStyleSheet("background-color:rgb(46,48,55);");
			Q_EMIT hideStylePanels();
		}
		else if (e->type() == QEvent::Leave && m_currentSelect != 1)
		{
			ui.widSelect->setStyleSheet("");
		}
	}
	else if (watched == ui.pbPen)
	{
		if (e->type() == QEvent::Enter)
		{
			ui.widPen->setStyleSheet("background-color:rgb(46,48,55);");
			Q_EMIT showPenStylePanel();
		}
		else if (e->type() == QEvent::Leave && m_currentSelect != 2)
		{
			ui.widPen->setStyleSheet("");
		}
	}
	else if (watched == ui.pbText)
	{
		if (e->type() == QEvent::Enter)
		{
			ui.widText->setStyleSheet("background-color:rgb(46,48,55);");
			Q_EMIT showTextStylePanel();
		}
		else if (e->type() == QEvent::Leave && m_currentSelect != 3)
		{
			ui.widText->setStyleSheet("");
		}
	}
	else if (watched == ui.pbCloud)
	{
		if (e->type() == QEvent::Enter)
		{
			ui.widCloud->setStyleSheet("background-color:rgb(46,48,55);");
			Q_EMIT hideStylePanels();
		}
		else if (e->type() == QEvent::Leave && !m_showCloudFileView)
		{
			ui.widCloud->setStyleSheet("");
		}
	}
	else if (watched == ui.pbDelete)
	{
		if (e->type() == QEvent::Enter)
		{
			ui.widDelete->setStyleSheet("background-color:rgb(46,48,55);");
			Q_EMIT hideStylePanels();
		}
		else if (e->type() == QEvent::Leave)
		{
			ui.widDelete->setStyleSheet("");
		}
	}
	else if (watched == ui.pbInvite)
	{
		if (e->type() == QEvent::Enter)
		{
			ui.widInvite->setStyleSheet("background-color:rgb(46,48,55);");
			Q_EMIT hideStylePanels();
		}
		else if (e->type() == QEvent::Leave && !m_showInviteFriend)
		{
			ui.widInvite->setStyleSheet("");
		}
	}
	else if (watched == ui.pbRoster)
	{
		if (e->type() == QEvent::Enter)
		{
			ui.widRoster->setStyleSheet("background-color:rgb(46,48,55);");
			Q_EMIT hideStylePanels();
		}
		else if (e->type() == QEvent::Leave && !m_showRoster)
		{
			ui.widRoster->setStyleSheet("");
		}
	}
	else if (watched == ui.pbDeleteCourse)
	{
		if (e->type() == QEvent::Enter)
		{
			ui.widDeleteCourse->setStyleSheet("background-color:rgb(46,48,55);");
			Q_EMIT hideStylePanels();
		}
		else if (e->type() == QEvent::Leave)
		{
			ui.widDeleteCourse->setStyleSheet("");
		}
	}

	return QWidget::eventFilter(watched, e);
}

void STWBVToolbar::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Escape:
		break;
	default:
		QDialog::keyPressEvent(event);
	}
}
