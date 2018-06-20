#ifndef _STSETTING_H_
#define _STSETTING_H_

#include <QDialog>
#include <QMouseEvent>
#include <QCameraInfo>
#include <QAudioDeviceInfo>
#include <QStyledItemDelegate>
#include <QCameraViewfinder>
#include "stconfig.h"
#include "ui_STSetting.h"

namespace tahiti
{
	class NoFocusFrameDelegate : public QStyledItemDelegate
	{
		Q_OBJECT
	public:
		explicit NoFocusFrameDelegate(QWidget *parent = 0);
		void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	};

	class STSetting : public QDialog
	{
		Q_OBJECT

	public:
		STSetting(QWidget * parent = 0);
		~STSetting();
		public Q_SLOTS:
		void on_pbConfirm_clicked();
		void on_pbClose_clicked();
		void on_pbConfirmDevice_clicked();
		void cameraValueChanged();
		void micValueChanged();
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
		virtual void keyPressEvent(QKeyEvent *event);
	private:
		static bool compareResolutionData(const QSize &size1, const QSize &size2);
	private:
		Ui::STSettingClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;
		QString m_cameraId;
		int m_width;
		int m_height;
		QString m_micId;
		QCamera* m_camera;
		QCameraViewfinder* m_viewfinder;
	};
}
#endif
