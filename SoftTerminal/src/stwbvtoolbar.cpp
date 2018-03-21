#include "stwbvtoolbar.h"

using namespace tahiti;

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

	m_currentSelect = 0;

	m_cloud_file_view = new STWBCloudFileView;
	QObject::connect(m_cloud_file_view, SIGNAL(closeCloudFileView()), this, SLOT(closeCloudFileView()));
	QObject::connect(m_cloud_file_view, SIGNAL(openCloudFileSignal(QString)), this, SLOT(openCloudFile(QString)));
	m_cloud_file_view->hide();
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
		m_currentSelect = 0;
	}
	else
	{
		ui.widSelect->setStyleSheet("background-color:rgba(46,48,55,0.9);");
		m_currentSelect = 1;
	}
	ui.widPen->setStyleSheet("");
	ui.widText->setStyleSheet("");
	Q_EMIT setActionMode(m_currentSelect);
}

void STWBVToolbar::on_pbPen_clicked()
{
	if (m_currentSelect == 2)
	{
		ui.widPen->setStyleSheet("");
		m_currentSelect = 0;
	}
	else
	{
		ui.widPen->setStyleSheet("background-color:rgba(46,48,55,0.9);");
		m_currentSelect = 2;
	}
	ui.widSelect->setStyleSheet("");
	ui.widText->setStyleSheet("");
	Q_EMIT setActionMode(m_currentSelect);
}

void STWBVToolbar::on_pbText_clicked()
{
	if (m_currentSelect == 3)
	{
		ui.widText->setStyleSheet("");
		m_currentSelect = 0;
	}
	else
	{
		ui.widText->setStyleSheet("background-color:rgba(46,48,55,0.9);");
		m_currentSelect = 3;
	}
	ui.widSelect->setStyleSheet("");
	ui.widPen->setStyleSheet("");
	Q_EMIT setActionMode(m_currentSelect);
}

void STWBVToolbar::on_pbCloud_clicked()
{
	ui.widCloud->setStyleSheet("background-color:rgba(46,48,55,0.9);");
	int x = (parentWidget()->geometry().width() - m_cloud_file_view->geometry().width()) / 2
		+ parentWidget()->geometry().x();
	int y = (parentWidget()->geometry().height() - m_cloud_file_view->geometry().height()) / 2
		+ parentWidget()->geometry().y();
	m_cloud_file_view->move(QPoint(x, y));
	m_cloud_file_view->setWindowModality(Qt::ApplicationModal);
	m_cloud_file_view->show();
	m_cloud_file_view->initCloudFileView();
}

void STWBVToolbar::on_pbDelete_clicked()
{
	Q_EMIT deleteAction();
}

void STWBVToolbar::openCloudFile(QString path)
{
	Q_EMIT openCloudFileSignal(path);
}

void STWBVToolbar::closeCloudFileView()
{
	ui.widCloud->setStyleSheet("");
	m_cloud_file_view->hide();
}

void STWBVToolbar::init()
{
	m_currentSelect = 0;
	Q_EMIT setActionMode(m_currentSelect);
	ui.widSelect->setStyleSheet("");
	ui.widPen->setStyleSheet("");
	ui.widText->setStyleSheet("");
}

bool STWBVToolbar::eventFilter(QObject* watched, QEvent* e)
{
	// 鼠标进入或离开关闭按钮时设置为不同状态的图标
	if (watched == ui.pbSelect)
	{
		if (e->type() == QEvent::Enter)
		{
			ui.widSelect->setStyleSheet("background-color:rgba(46,48,55,0.9);");
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
			ui.widPen->setStyleSheet("background-color:rgba(46,48,55,0.9);");
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
			ui.widText->setStyleSheet("background-color:rgba(46,48,55,0.9);");
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
			ui.widCloud->setStyleSheet("background-color:rgba(46,48,55,0.9);");
			Q_EMIT hideStylePanels();
		}
		else if (e->type() == QEvent::Leave && m_cloud_file_view->isHidden())
		{
			ui.widCloud->setStyleSheet("");
		}
	}
	else if (watched == ui.pbDelete)
	{
		if (e->type() == QEvent::Enter)
		{
			ui.widDelete->setStyleSheet("background-color:rgba(46,48,55,0.9);");
			Q_EMIT hideStylePanels();
		}
		else if (e->type() == QEvent::Leave)
		{
			ui.widDelete->setStyleSheet("");
		}
	}

	return QWidget::eventFilter(watched, e);
}