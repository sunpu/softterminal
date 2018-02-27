#ifndef _STCONTACTITEM_H_
#define _STCONTACTITEM_H_

#include <QWidget>
#include "ui_STContactItem.h"
#include "stcommon.h"

using namespace tahiti;

namespace tahiti
{
	class STContactItem : public QWidget
	{
		Q_OBJECT

	public:
		STContactItem(QWidget* parent = 0);
		~STContactItem();
		void setUserInfo(UserInfo userInfo);
		UserInfo getUserInfo();
	private:
		Ui::STContactItemClass ui;
		UserInfo m_userInfo;
	};
}
#endif
