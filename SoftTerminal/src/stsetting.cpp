#include "stsetting.h"

using namespace tahiti;

STSetting::STSetting(QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

	QString server = STConfig::getConfig("/xmpp/server");
	ui.leServerIP->setText(server);

	ui.cbxCamera->setItemDelegate(new NoFocusFrameDelegate(this));

	/*QList<QAudioDeviceInfo> audioInputs = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
	QList<QAudioDeviceInfo>::Iterator it1;
	QString audioInputId;
	for (it1 = audioInputs.begin(); it1 != audioInputs.end(); it1++)
	{
		audioInputId = it1->deviceName();
	}
	QList<QAudioDeviceInfo> audioOutputs = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
	QList<QAudioDeviceInfo>::Iterator it2;
	QString audioOutputId;
	for (it2 = audioOutputs.begin(); it2 != audioOutputs.end(); it2++)
	{
		audioOutputId = it2->deviceName();
	}*/
	QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
	QList<QCameraInfo>::Iterator it;
	QString cameraId;
	QString cameraDes;
	QVariant value;
	value.setValue(QString(""));
	ui.cbxCamera->addItem(QStringLiteral("禁用"), value);
	for (it = cameras.begin(); it != cameras.end(); it++)
	{
		cameraId = it->deviceName();
		cameraDes = it->description();
		value.setValue(cameraId);
		ui.cbxCamera->addItem(cameraDes, value);
	}

	connect(ui.cbxCamera, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxValueChanged()));
	m_camera = NULL;

	m_cameraId = STConfig::getConfig("/camera/id");
	m_width = STConfig::getConfig("/camera/width").toInt();
	m_height = STConfig::getConfig("/camera/height").toInt();
	if (m_cameraId.size() == 0)
	{
		comboBoxValueChanged();
	}
	else
	{
		int idx = ui.cbxCamera->findData(m_cameraId);
		if (idx == -1) idx = 0;
		ui.cbxCamera->setCurrentIndex(idx);
	}
}

STSetting::~STSetting()
{

}

void STSetting::on_pbConfirm_clicked()
{
	STConfig::setConfig("/xmpp/server", ui.leServerIP->text());
	on_pbClose_clicked();
}

void STSetting::on_pbClose_clicked()
{
	if (m_camera != NULL)
	{
		m_camera->stop();
	}
	close();
}

bool STSetting::compareResolutionData(const QSize &size1, const QSize &size2)
{
	if (size1.width() <= size2.width() && size1.height() <= size2.height())
	{
		return true;
	}
	return false;
}

void STSetting::comboBoxValueChanged()
{
	if (m_camera != NULL)
	{
		m_camera->stop();
		delete m_camera;
		m_camera = NULL;
		delete m_viewfinder;
		m_viewfinder = NULL;
	}
	m_cameraId = ui.cbxCamera->currentData().value<QString>();
	if (m_cameraId.size() == 0)
	{
		ui.widPreview->layout()->removeWidget(m_viewfinder);
		m_width = 0;
		m_height = 0;
		return;
	}

	m_camera = new QCamera(m_cameraId.toLatin1());
	m_camera->load();
	QList<QSize> resolutions = m_camera->supportedViewfinderResolutions();
	qSort(resolutions.begin(), resolutions.end(), compareResolutionData);

	QList<QSize>::Iterator it;
	for (it = resolutions.begin(); it != resolutions.end(); it++)
	{
		if (it->width() > 640 && it->height() > 360)
		{
			break;
		}
		m_width = it->width();
		m_height = it->height();
	}
	m_camera->unload();

	m_viewfinder = new QCameraViewfinder(this);
	ui.widPreview->layout()->addWidget(m_viewfinder);
	m_camera->setViewfinder(m_viewfinder);
	m_camera->start();
}

void STSetting::on_pbConfirmCamera_clicked()
{
	STConfig::setConfig("/camera/id", m_cameraId);
	STConfig::setConfig("/camera/width", QString::number(m_width));
	STConfig::setConfig("/camera/height", QString::number(m_height));
	on_pbClose_clicked();
}

void STSetting::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}
}

void STSetting::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint pos = this->pos() + movePoint;
		m_startMovePos = event->globalPos();
		this->move(pos.x(), pos.y());
	}
}

void STSetting::mouseReleaseEvent(QMouseEvent* event)
{
	m_isPressed = false;
}

void STSetting::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Escape:
        break;
    default:
        QDialog::keyPressEvent(event);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
NoFocusFrameDelegate::NoFocusFrameDelegate(QWidget *parent) :
	QStyledItemDelegate(parent)
{
}

void NoFocusFrameDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItem itemOption(option);
	if (itemOption.state & QStyle::State_HasFocus)
	{
		itemOption.state ^= QStyle::State_HasFocus;
	}
	QStyledItemDelegate::paint(painter, itemOption, index);
}
