#ifndef STWBVTOOLBAR_H
#define STWBVTOOLBAR_H

#include <QWidget>
#include <QDialog>
#include <QEvent>
#include "ui_STWBVToolbar.h"
#include "ui_STWBPenStylePanel.h"
#include "ui_STWBTextStylePanel.h"

namespace tahiti
{
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
		public Q_SLOTS:
		void on_pbSelect_clicked();
		void on_pbPen_clicked();
		void on_pbText_clicked();
		void on_pbDelete_clicked();
	Q_SIGNALS:
		void setActionMode(int mode);
		void deleteAction();
		void hideStylePanels();
		void showPenStylePanel();
		void showTextStylePanel();

	private:
		Ui::STWBVToolbarClass ui;
		int m_currentSelect;
	};
}
#endif
