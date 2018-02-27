#include "stchatrecorditem.h"

using namespace tahiti;

STChatRecordItem::STChatRecordItem(RecordItem item)
{
	ui.setupUi(this);

	m_width = 0;
	m_height = 0;
	int width = 0;
	int height = 0;
	QStringList txtLines = item.content.split("\n");
	for (int i = 0; i < txtLines.size(); i++)
	{
		QStaticText txt(txtLines[i]);
		txt.setTextFormat(Qt::PlainText);
		width = ui.teMessageSelf->fontMetrics().width(txt.text());
		height = ui.teMessageSelf->fontMetrics().height();
		if (width > 200)
		{
			int lines = width / 200 + 1;
			height = lines * height;
			width = 200;
		}
		if (width > m_width)
		{
			m_width = width;
		}
		m_height += height;
	}

	ui.lblTime->setText(item.time);

	QString path = item.pic;
	if (path.size() == 0)
	{
		path = ":/SoftTerminal/images/account.png";
	}
	QImage* image = new QImage(path);

	if (item.from == MessageFrom::Self)
	{
		ui.lblChatRecordPicSelf->setPixmap(QPixmap::fromImage(*image).scaled(36, 36));
		ui.teMessageSelf->setMaximumWidth(m_width + 22);
		ui.teMessageSelf->setMaximumHeight(m_height + 22);
		ui.teMessageSelf->setText(item.content);

		ui.lblChatRecordPicOther->setVisible(false);
		ui.teMessageOther->setVisible(false);
		ui.lblChatRecordPicSelf->setVisible(true);
		ui.teMessageSelf->setVisible(true);
	}
	else
	{
		ui.lblChatRecordPicOther->setPixmap(QPixmap::fromImage(*image).scaled(36, 36));
		ui.teMessageOther->setMaximumWidth(m_width + 22);
		ui.teMessageOther->setMaximumHeight(m_height + 22);
		ui.teMessageOther->setText(item.content);

		ui.lblChatRecordPicOther->setVisible(true);
		ui.teMessageOther->setVisible(true);
		ui.lblChatRecordPicSelf->setVisible(false);
		ui.teMessageSelf->setVisible(false);
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

QSize STChatRecordItem::getItemSize()
{
	return QSize(m_width, m_height);
}
