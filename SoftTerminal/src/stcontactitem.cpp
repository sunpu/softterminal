#include "stcontactitem.h"

using namespace tahiti;

STContactItem::STContactItem(bool littleMode,
	bool showAddBtn, bool showDeleteBtn, bool showAdmin, QWidget* parent)
	: QWidget(parent), m_littleMode(littleMode)
{
	ui.setupUi(this);

	ui.pbAdd->setVisible(showAddBtn);
	ui.pbDelete->setVisible(showDeleteBtn);
	ui.lblAdmin->setVisible(showAdmin);

	if (m_littleMode)
	{
		setFixedHeight(30);
		ui.lblContactPic->setFixedSize(24, 24);
		QFont font;
		font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
		font.setPointSize(10);
		ui.lblContactName->setFont(font);
	}
}

STContactItem::~STContactItem()
{

}

void STContactItem::setUserInfo(UserInfo userInfo, bool offline)
{
	m_userInfo = userInfo;

	if (userInfo.userName.isEmpty())
	{
		ui.lblContactName->setText(userInfo.jid);
	}
	else
	{
		ui.lblContactName->setText(userInfo.userName);
	}

	QString path = userInfo.photoPath;
	if (path.size() == 0)
	{
		path = ":/SoftTerminal/images/account.png";
	}
	QImage* image = new QImage(path);
	QImage* showImage = image;
	if (offline)
	{
		int width = image->width();
		int height = image->height();
		int bytePerLine = (width * 24 + 31) / 8;
		unsigned char* data = image->bits();
		unsigned char* graydata = new unsigned char[bytePerLine * height];

		unsigned char r, g, b;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				r = *(data + 2);
				g = *(data + 1);
				b = *data;

				graydata[i * bytePerLine + j * 3] = (r * 30 + g * 59 + b * 11) / 100;
				graydata[i * bytePerLine + j * 3 + 1] = (r * 30 + g * 59 + b * 11) / 100;
				graydata[i * bytePerLine + j * 3 + 2] = (r * 30 + g * 59 + b * 11) / 100;

				data += 4;
			}
		}
		showImage = new QImage(graydata, width, height, bytePerLine, QImage::Format_RGB888);
	}
	if (m_littleMode)
	{
		ui.lblContactPic->setPixmap(QPixmap::fromImage(*showImage).scaled(24, 24));
	}
	else
	{
		ui.lblContactPic->setPixmap(QPixmap::fromImage(*showImage).scaled(36, 36));
	}
}

UserInfo STContactItem::getUserInfo()
{
	return m_userInfo;
}

void STContactItem::on_pbAdd_clicked()
{
	Q_EMIT addFriendSignal(m_userInfo);
}

void STContactItem::on_pbDelete_clicked()
{
	Q_EMIT deleteMemberSignal(m_userInfo);
}
