#include "stuploadpic.h"

using namespace tahiti;

STUploadPic::STUploadPic(QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);
	QString path = ":/SoftTerminal/images/account.png";
	QImage* image = new QImage(path);
	ui.lblPic->setPixmap(QPixmap::fromImage(*image).scaled(80, 80));
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
}

STUploadPic::~STUploadPic()
{

}

void STUploadPic::on_pbUpload_clicked()
{
	char *image_filter =
		"All formats (*.png *.jpeg *.jpg);;"
		"PNG Files (*.png);;"
		"JPEG Files (*.jpeg *.jpg)";

	m_picPath = QFileDialog::getOpenFileName(this,
		QString("选择图片"), QString(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)),
		QString(image_filter));

	if (m_picPath.isEmpty() || !QFile::exists(m_picPath))
	{
		return;
	}

	QImage* image = new QImage(m_picPath);
	ui.lblPic->setPixmap(QPixmap::fromImage(*image).scaled(80, 80));
}

void STUploadPic::on_pbOK_clicked()
{
	if (m_picPath.size() != 0)
	{
		Q_EMIT uploadPic(m_picPath);
	}
	close();
}

void STUploadPic::on_pbCancel_clicked()
{
	close();
}

void STUploadPic::on_pbClose_clicked()
{
	close();
}

void STUploadPic::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}
}

void STUploadPic::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint pos = this->pos() + movePoint;
		m_startMovePos = event->globalPos();
		this->move(pos.x(), pos.y());
	}
}

void STUploadPic::mouseReleaseEvent(QMouseEvent* event)
{
	m_isPressed = false;
}
