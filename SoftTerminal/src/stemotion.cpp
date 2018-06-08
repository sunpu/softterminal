#include "stemotion.h"

using namespace tahiti;

STEmotionItem::STEmotionItem(QString fileName)
	: QLabel(NULL)
{
	setStyleSheet("QLabel:hover{border: 1px solid #e3e3e3;}");
	QImage* image = new QImage(fileName);
	setPixmap(QPixmap::fromImage(*image).scaled(24, 24));
}

STEmotionItem::~STEmotionItem()
{
}

void STEmotionItem::enterEvent(QEvent* event)
{
	return __super::enterEvent(event);
}

void STEmotionItem::leaveEvent(QEvent* event)
{
	return __super::leaveEvent(event);
}

// ------------------------ STEmotion ------------------------
STEmotion::STEmotion(QWidget * parent) : QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Popup);
	ui.twEmotion->setEditTriggers(QTableWidget::NoEditTriggers);
	connect(ui.twEmotion, SIGNAL(cellClicked(int, int)), this, SLOT(emotionSelected(int, int)));

	QString path = ":/SoftTerminal/emoji/%1.jpg";
	for (int i = 0; i < 40; i++)
	{
		addEmotionItem(path.arg(i + 1), i);
	}
}

STEmotion::~STEmotion()
{

}

void STEmotion::emotionSelected(int row, int column)
{
	int index = row * 8 + column + 1;
	Q_EMIT chooseEmotion(index);
	this->close();
}

void STEmotion::addEmotionItem(QString fileName, int index)
{
	int row = m_emotionList.size() / ui.twEmotion->columnCount();
	int column = m_emotionList.size() % ui.twEmotion->columnCount();

	QTableWidgetItem* tableWidgetItem = new QTableWidgetItem;
	tableWidgetItem->setToolTip(QString::fromLocal8Bit(EmotionName[index].toLocal8Bit().data()));
	ui.twEmotion->setItem(row, column, tableWidgetItem);

	STEmotionItem* emotionItem = new STEmotionItem(fileName);
	QWidget* widget = new QWidget();
	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->addWidget(emotionItem);
	hLayout->setMargin(0);
	hLayout->setAlignment(emotionItem, Qt::AlignCenter);
	widget->setLayout(hLayout);
	ui.twEmotion->setCellWidget(row, column, widget);
	m_emotionList.push_back(fileName);
}

bool STEmotion::event(QEvent* event)
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
