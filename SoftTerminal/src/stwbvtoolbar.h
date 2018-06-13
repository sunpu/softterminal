#ifndef _STWBVTOOLBAR_H
#define _STWBVTOOLBAR_H

#include <QWidget>
#include <QDialog>
#include <QEvent>
#include <QKeyEvent>
#include "ui_STWBVToolbar.h"
#include "ui_STWBPenStylePanel.h"
#include "ui_STWBTextStylePanel.h"
#include "ui_STWBRaiseHandPanel.h"

namespace tahiti
{
	class STWBRaiseHandPanel : public QDialog
	{
		Q_OBJECT

	public:
		STWBRaiseHandPanel(QWidget * parent = 0);
		~STWBRaiseHandPanel();
		private Q_SLOTS:
		void on_pbRaiseHand_clicked();
	Q_SIGNALS:
		void raiseHandSignal(bool raise);

	private:
		Ui::STWBRaiseHandPanelClass ui;
		bool m_isRaised;
	};

	class STWBPenStylePanel : public QDialog
	{
		Q_OBJECT

	public:
		STWBPenStylePanel(QWidget * parent = 0);
		~STWBPenStylePanel();
		void init();
	protected:
		bool eventFilter(QObject* watched, QEvent* e);
		private Q_SLOTS:
		void on_pb1_clicked();
		void on_pb2_clicked();
		void on_pb3_clicked();
		void on_pb4_clicked();
	Q_SIGNALS:
		void updatePenThickness(int thickness);
		void updatePenColor(QString color);

	private:
		Ui::STWBPenStylePanelClass ui;
		int m_currentSelect;
	};

	class STWBTextStylePanel : public QDialog
	{
		Q_OBJECT

	public:
		STWBTextStylePanel(QWidget * parent = 0);
		~STWBTextStylePanel();
		void init();
	protected:
		bool eventFilter(QObject* watched, QEvent* e);
		private Q_SLOTS:
		void on_pb1_clicked();
		void on_pb2_clicked();
		void on_pb3_clicked();
		void on_pb4_clicked();
	Q_SIGNALS:
		void updateTextSize(int size);
		void updateTextColor(QString color);

	private:
		Ui::STWBTextStylePanelClass ui;
		int m_currentSelect;
	};

	class STWBVToolbar : public QDialog
	{
		Q_OBJECT

	public:
		STWBVToolbar(QWidget * parent = 0);
		~STWBVToolbar();
		void changePenShowColor(QString color);
		void changeTextShowColor(QString color);
		int getCurrentSelect() { return m_currentSelect; }
		void init();
	protected:
		bool eventFilter(QObject* watched, QEvent* e);
		virtual void keyPressEvent(QKeyEvent *event);
		void paintEvent(QPaintEvent *);
		public Q_SLOTS:
		void on_pbSelect_clicked();
		void on_pbPen_clicked();
		void on_pbText_clicked();
		void on_pbDelete_clicked();
		void on_pbCloud_clicked();
		void on_pbInvite_clicked();
		void on_pbRoster_clicked();
		void on_pbDeleteCourse_clicked();
		void closeCloudFileView();
		void closeInviteFriend();
		void closeRoster();
		void listenModeSlot();
	Q_SIGNALS:
		void openCloudFileViewSignal();
		void openInviteFriendSignal();
		void openRosterSignal();
		void closeCloudFileViewSignal();
		void closeInviteFriendSignal();
		void closeRosterSignal();
		void deleteCourseSignal();
		void setActionMode(int mode);
		void deleteAction();
		void hideStylePanels();
		void showPenStylePanel();
		void showTextStylePanel();

	private:
		Ui::STWBVToolbarClass ui;
		int m_currentSelect;
		bool m_showCloudFileView;
		bool m_showInviteFriend;
		bool m_showRoster;
	};
}
#endif
