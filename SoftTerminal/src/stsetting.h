#ifndef _STSETTING_H_
#define _STSETTING_H_

#include <QDialog>
#include <QMouseEvent>
#include "stconfig.h"
#include "ui_STSetting.h"

namespace tahiti
{
	class STSetting : public QDialog
	{
		Q_OBJECT

	public:
		STSetting(QWidget * parent = 0);
		~STSetting();
		public Q_SLOTS:
		void on_pbConfirm_clicked();
		void on_pbClose_clicked();
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);

	private:
		Ui::STSettingClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;

	};
}
#endif
