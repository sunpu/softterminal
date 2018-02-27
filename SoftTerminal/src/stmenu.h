#ifndef _STMENU_H_
#define _STMENU_H_

#include <QWidget>
#include <QEvent>
#include <windows.h>
#include <Winuser.h>
#include "ui_STMenu.h"
#include "stconfirm.h"

namespace tahiti
{
	class STMenu : public QWidget
	{
		Q_OBJECT

	public:
		STMenu(QWidget* parent = 0);
		~STMenu();
	protected:
		bool eventFilter(QObject* obj, QEvent* e);
		bool event(QEvent* event);
	Q_SIGNALS:
		void confirmExit();
		void confirmRelogin();
		void showSettingWindow();
	private:
		Ui::STMenuClass ui;

	};
}
#endif
