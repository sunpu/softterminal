#ifndef _STUPLOADPIC_H_
#define _STUPLOADPIC_H_

#include <QDialog>
#include <QMouseEvent>
#include <QFileDialog>
#include <QStandardPaths>
#include <QPainter>
#include <QtMath>
#include "ui_STUploadPic.h"

namespace tahiti
{
	class STUploadPic : public QDialog
	{
		Q_OBJECT

	public:
		STUploadPic(QWidget * parent = 0);
		~STUploadPic();
		public Q_SLOTS:
		void on_pbUpload_clicked();
		void on_pbOK_clicked();
		void on_pbCancel_clicked();
		void on_pbClose_clicked();
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
	Q_SIGNALS:
		void uploadPic(QString path);

	private:
		Ui::STUploadPicClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;
		QString m_picPath;
	};
}
#endif
