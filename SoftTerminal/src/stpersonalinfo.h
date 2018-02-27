#ifndef _STPERSONALINFO_H_
#define _STPERSONALINFO_H_

#include <QWidget>
#include <QDirIterator>
#include "ui_STPersonalInfo.h"
#include "xmppclient.h"
#include "stuploadpic.h"

namespace tahiti
{
	class STPersonalInfo : public QWidget
	{
		Q_OBJECT

	public:
		STPersonalInfo(XmppClient* client);
		~STPersonalInfo();
		void initPersonalInfo();
		public Q_SLOTS:
		void on_pbEdit_clicked();
		void on_pbSave_clicked();
		void on_pbModifyPasswd_clicked();
		void on_pbModify_clicked();
		void on_pbCancel_clicked();
		void updatePersonalInfoData(UserInfo info);
	protected:
		bool eventFilter(QObject* obj, QEvent* e);
	private:
		void setPersonalInfoEditable(bool editable);
		void setModifyPasswdEditable(bool editable);
	Q_SIGNALS:
		void personalInfoFinished();
		void updateSelfPic(QString picPath);
		private Q_SLOTS:
		void uploadPicFinished(QString path);
	private:
		Ui::STPersonalInfoClass ui;
		XmppClient* m_xmppClient;
		UserInfo m_userInfo;
		MaskLabel* m_cameraPic;
	};
}
#endif
