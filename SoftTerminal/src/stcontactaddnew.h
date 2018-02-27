#ifndef _STCONTACTADDNEW_H_
#define _STCONTACTADDNEW_H_

#include <QWidget>
#include <QKeyEvent>
#include "ui_STContactAddNew.h"
#include "xmppclient.h"

using namespace tahiti;

namespace tahiti
{
	class STContactAddNew : public QWidget
	{
		Q_OBJECT

	public:
		STContactAddNew(XmppClient* client);
		~STContactAddNew();
		void initAddNewWindow();
		public Q_SLOTS:
		void on_pbSearch_clicked();
		void on_pbAddNew_clicked();
		private Q_SLOTS:
		void onContactFoundResult(int result, QVariant dataVar);
	protected:
		bool eventFilter(QObject *obj, QEvent *e);

	private:
		Ui::STContactAddNewClass ui;
		XmppClient* m_xmppClient;

	};
}
#endif
