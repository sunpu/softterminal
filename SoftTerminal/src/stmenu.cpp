#include "stmenu.h"

using namespace tahiti;

STMenu::STMenu(QWidget * parent) : QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Popup);

	ui.widParam->installEventFilter(this);
	ui.widRelogin->installEventFilter(this);
	ui.widExit->installEventFilter(this);
}

STMenu::~STMenu()
{

}

bool STMenu::eventFilter(QObject* obj, QEvent* e)
{
	if (e->type() == QEvent::Enter)
	{
		if (ui.widParam == obj)
		{
			ui.widParam->setStyleSheet("background:#ececec;");
		}
		else if (ui.widRelogin == obj)
		{
			ui.widRelogin->setStyleSheet("background:#ececec;");
		}
		else if (ui.widExit == obj)
		{
			ui.widExit->setStyleSheet("background:#ececec;");
		}
		return true;
	}
	else if (e->type() == QEvent::Leave)
	{
		if (ui.widParam == obj)
		{
			ui.widParam->setStyleSheet("");
		}
		else if (ui.widRelogin == obj)
		{
			ui.widRelogin->setStyleSheet("");
		}
		else if (ui.widExit == obj)
		{
			ui.widExit->setStyleSheet("");
		}
		return true;
	}
	else if (e->type() == QEvent::MouseButtonPress)
	{
		if (ui.widParam == obj)
		{
			Q_EMIT showSettingWindow();
		}
		else if (ui.widRelogin == obj)
		{
			Q_EMIT confirmRelogin();
		}
		else if (ui.widExit == obj)
		{
			Q_EMIT confirmExit();
		}
		return true;
	}
	return false;
}

bool STMenu::event(QEvent* event)
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
