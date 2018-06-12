#include "stchatrecorditem.h"

using namespace tahiti;

STChatRecordItem::STChatRecordItem(RecordItem item)
{
	ui.setupUi(this);

	QDateTime dateTime = QDateTime::fromString(item.time, "yyyy-MM-dd hh:mm:ss");
	QString showTime;
	if (dateTime.date() == QDate::currentDate())
	{
		QTime t = dateTime.time();
		showTime = dateTime.time().toString("hh:mm:ss");
	}
	else if (dateTime.date().year() == QDate::currentDate().year())
	{
		showTime = dateTime.toString("M-dd hh:mm:ss");
	}
	else
	{
		showTime = dateTime.toString("yyyy-MM-dd hh:mm:ss");
	}
	ui.lblTime->setText(showTime);

	if (item.type == MessageType::MT_Text)
	{
		ui.widCourse->setVisible(false);
		m_width = 0;
		m_height = 0;
		m_extraWidth = 0;
		int width = 0;
		int height = 0;
		QStringList txtLines = item.content.split("\n");
		for (int i = 0; i < txtLines.size(); i++)
		{
			QStaticText txt(txtLines[i]);
			txt.setTextFormat(Qt::PlainText);
			width = ui.teMessageSelf->fontMetrics().width(txt.text());
			height = ui.teMessageSelf->fontMetrics().height();
			int lines = 0;
			int leftTextWidth = 0;
			if (width > 330)
			{
				m_extraWidth = 20;
			}
			if (width > 350)
			{
				lines = width / 350 + 1;
				leftTextWidth = width % 350;
				if (lines > 1 && leftTextWidth <= 20)
				{
					lines--;
				}
				height = lines * height;
				width = 350;
			}
			if (width > m_width)
			{
				m_width = width;
			}
			m_height += height;
		}

		QString path = item.pic;
		if (!QFile::exists(path))
		{
			path = ":/SoftTerminal/images/account.png";
		}
		QImage* image = new QImage(path);

		ui.lblUserName->setVisible(false);
		if (item.from == MessageFrom::Self)
		{
			ui.lblChatRecordPicSelf->setPixmap(QPixmap::fromImage(*image).scaled(36, 36));
			ui.teMessageSelf->setFixedWidth(m_width + 10 + m_extraWidth);
			ui.teMessageSelf->setFixedHeight(m_height + 10);
			ui.teMessageSelf->setText(item.content);

			ui.lblChatRecordPicOther->setVisible(false);
			ui.teMessageOther->setVisible(false);
			ui.lblChatRecordPicSelf->setVisible(true);
			ui.teMessageSelf->setVisible(true);
		}
		else
		{
			if (!item.jid.isEmpty())
			{
				ui.lblUserName->setText(item.jid);
				ui.lblUserName->setVisible(true);
				m_height += 20;
			}
			ui.lblChatRecordPicOther->setPixmap(QPixmap::fromImage(*image).scaled(36, 36));
			ui.teMessageOther->setFixedWidth(m_width + 10 + m_extraWidth);
			ui.teMessageOther->setFixedHeight(m_height + 10);
			ui.teMessageOther->setText(item.content);

			ui.lblChatRecordPicOther->setVisible(true);
			ui.teMessageOther->setVisible(true);
			ui.lblChatRecordPicSelf->setVisible(false);
			ui.teMessageSelf->setVisible(false);
		}
	}
	else if (item.type == MessageType::MT_CourseCreate)
	{
		ui.widText->setVisible(false);
		ui.lblCourseOwner->setText(item.jid);

		showTime = dateTime.toString("yyyy-MM-dd hh:mm");
		ui.lblCourseStartTime->setText(showTime);
		ui.lblCourseStopTime->setText("-");
		ui.lblCourseEnd->setVisible(false);

		m_width = 300;
		m_height = 130;
	}
}

STChatRecordItem::~STChatRecordItem()
{

}

void STChatRecordItem::updateSelfPic(QString picPath)
{
	QImage* image = new QImage(picPath);
	ui.lblChatRecordPicSelf->setPixmap(QPixmap::fromImage(*image).scaled(36, 36));
}

void STChatRecordItem::updateCourseDelete(QString deleteTime)
{
	if (!ui.widCourse->isHidden())
	{
		QDateTime dateTime = QDateTime::fromString(deleteTime, "yyyy-MM-dd hh:mm:ss");
		QString showTime = dateTime.toString("yyyy-MM-dd hh:mm");
		ui.lblCourseStopTime->setText(showTime);
		ui.lblCourseEnd->setVisible(true);
		ui.pbEnterCourse->setVisible(false);
	}
}

void STChatRecordItem::on_pbEnterCourse_clicked()
{
	Q_EMIT joinCourseSignal();
}

QSize STChatRecordItem::getItemSize()
{
	return QSize(m_width, m_height);
}
