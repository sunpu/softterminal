#include "stpersonalinfo.h"

using namespace tahiti;

STPersonalInfo::STPersonalInfo(XmppClient* client)
	: m_xmppClient(client)
{
	ui.setupUi(this);
	ui.lblPic->installEventFilter(this);
	m_cameraPic = new MaskLabel(ui.lblPic);
	QImage* image = new QImage(":/SoftTerminal/images/camera.png");
	m_cameraPic->setPixmap(QPixmap::fromImage(*image).scaled(80, 80));
	m_cameraPic->hide();
}

STPersonalInfo::~STPersonalInfo()
{

}

void STPersonalInfo::initPersonalInfo()
{
	setPersonalInfoEditable(false);
	setModifyPasswdEditable(false);

	m_userInfo = m_xmppClient->getSelfInfo();
	updatePersonalInfoData(m_userInfo);
}

void STPersonalInfo::updatePersonalInfoData(UserInfo info)
{
	QString path = info.photoPath;
	if (path.size() == 0)
	{
		path = ":/SoftTerminal/images/account.png";
	}
	QImage* image = new QImage(path);
	ui.lblPic->setPixmap(QPixmap::fromImage(*image).scaled(80, 80));

	ui.lblName->setText(info.userName);
	ui.leName->setText(info.userName);

	ui.lblJid->setText(info.jid);

	ui.lblTelephone->setText(info.telephone);
	ui.leTelephone->setText(info.telephone);

	ui.lblEmail->setText(info.email);
	ui.leEmail->setText(info.email);

	ui.lblDescription->setText(info.description);
	ui.leDescription->setText(info.description);
}

void STPersonalInfo::setPersonalInfoEditable(bool editable)
{
	ui.lblName->setVisible(!editable);
	ui.leName->setVisible(editable);
	ui.lblTelephone->setVisible(!editable);
	ui.leTelephone->setVisible(editable);
	ui.lblEmail->setVisible(!editable);
	ui.leEmail->setVisible(editable);
	ui.lblDescription->setVisible(!editable);
	ui.leDescription->setVisible(editable);
	ui.pbEdit->setVisible(!editable);
	ui.pbSave->setVisible(editable);
}

void STPersonalInfo::setModifyPasswdEditable(bool editable)
{
	ui.widModifyPasswd->setVisible(editable);
	ui.widModifyAccess->setVisible(!editable);
}

void STPersonalInfo::on_pbEdit_clicked()
{
	setPersonalInfoEditable(true);
}

void STPersonalInfo::on_pbSave_clicked()
{
	m_userInfo.userName = ui.leName->text();
	m_userInfo.telephone = ui.leTelephone->text();
	m_userInfo.email = ui.leEmail->text();
	m_userInfo.description = ui.leDescription->text();

	updatePersonalInfoData(m_userInfo);
	setPersonalInfoEditable(false);
	m_xmppClient->modifyVCard(m_userInfo);
}

void STPersonalInfo::on_pbModifyPasswd_clicked()
{
	setModifyPasswdEditable(true);
}

void STPersonalInfo::on_pbModify_clicked()
{
	setModifyPasswdEditable(false);
	m_xmppClient->modifyPasswd(ui.lePasswd->text());
}

void STPersonalInfo::on_pbCancel_clicked()
{
	setModifyPasswdEditable(false);
	ui.leOldPasswd->clear();
	ui.lePasswd->clear();
	ui.lePasswdVerify->clear();
}

bool STPersonalInfo::eventFilter(QObject* obj, QEvent* e)
{
	if (ui.lblPic == obj)
	{
		if (e->type() == QEvent::Enter)
		{
			m_cameraPic->show();
			return true;
		}
		else if (e->type() == QEvent::Leave)
		{
			m_cameraPic->hide();
			return true;
		}
		else if (e->type() == QEvent::MouseButtonPress)
		{
			STUploadPic* uploadPic = new STUploadPic(this);
			connect(uploadPic, SIGNAL(uploadPic(QString)), this, SLOT(uploadPicFinished(QString)));
			uploadPic->setGeometry(mapToGlobal(pos()).x() + (width() - uploadPic->width()) / 2,
				mapToGlobal(pos()).y() + (height() - uploadPic->height()) / 2,
				uploadPic->width(), uploadPic->height());
			uploadPic->exec();
			return true;
		}
	}
	return false;
}

void STPersonalInfo::uploadPicFinished(QString path)
{
	QString desPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
		+ DATA_ROOT_PATH + AVATAR_PATH;

	// 删除旧图片
	QString oldFileName;
	QDir dir;
	dir.setPath(desPath);
	QDirIterator iter(dir, QDirIterator::Subdirectories);
	while (iter.hasNext())
	{
		iter.next();
		QFileInfo info = iter.fileInfo();
		if (info.isFile() && info.fileName().startsWith(m_userInfo.jid))
		{
			oldFileName = QString(desPath) + info.fileName();
			QFile::remove(oldFileName);
		}
	}

	// 复制新图片
	QFileInfo fi = QFileInfo(path);
	QString baseName = fi.suffix().toLower();
	QString desFile = desPath + m_userInfo.jid + "." + baseName;
	QFile::copy(path, desFile);

	// 更新
	m_userInfo.photoPath = desFile;
	updatePersonalInfoData(m_userInfo);

	m_xmppClient->modifySelfPic(desFile);

	Q_EMIT updateSelfPic(desFile);
}
