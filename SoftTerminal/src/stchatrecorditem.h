#ifndef _STCHATRECORDITEM_H
#define _STCHATRECORDITEM_H

#include <QWidget>
#include <QStaticText>
#include <QDateTime>
#include <QFile>
#include "ui_STChatRecordItem.h"
#include "stcommon.h"

using namespace tahiti;

namespace tahiti
{
	class STChatRecordItem : public QWidget
	{
		Q_OBJECT

	public:
		STChatRecordItem(RecordItem item);
		~STChatRecordItem();
		void updateSelfPic(QString picPath);
		QSize getItemSize();
		void updateCourseDelete(QString deleteTime);
		public Q_SLOTS:
		void on_pbEnterCourse_clicked();
	Q_SIGNALS:
		void joinCourseSignal();
	private:
	private:
		Ui::STChatRecordItemClass ui;
		int m_width;
		int m_height;
		int m_extraWidth;
	};
}
#endif
