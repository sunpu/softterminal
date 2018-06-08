#ifndef _STCONFIRM_H_
#define _STCONFIRM_H_

#include <QDialog>
#include <QMouseEvent>
#include "ui_STConfirm.h"

namespace tahiti
{
	class STConfirm : public QDialog
	{
		Q_OBJECT

	public:
		STConfirm(bool alert = false, QWidget * parent = 0);
		~STConfirm();
		void setText(QString text);
		public Q_SLOTS:
		void on_pbOK_clicked();
		void on_pbCancel_clicked();
		void on_pbClose_clicked();
		void on_pbConfirm_clicked();
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
	Q_SIGNALS:
		void confirmOK();
	private:
		Ui::STConfirmClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;

	};
}
#endif
