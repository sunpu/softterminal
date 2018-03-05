#ifndef _STWBDOCWINDOW_H_
#define _STWBDOCWINDOW_H_

#include <QDialog>
#include <QMouseEvent>
#include "ui_STWBDocWindow.h"

namespace tahiti
{
	class STWBDocWindow : public QDialog
	{
		Q_OBJECT

	public:
		STWBDocWindow(QWidget * parent = 0);
		~STWBDocWindow();
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);

	private:
		Ui::STWBDocWindowClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;

	};
}
#endif
