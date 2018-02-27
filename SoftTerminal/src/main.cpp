#include "stcommon.h"
#include "stsample.h"
#include "stconfig.h"
#include "stloginrotate.h"
#include <QtWidgets/QApplication>

using namespace tahiti;

int main(int argc, char *argv[])
{
	// ��ʼ�����ݴ洢λ��
	QString rootPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
		+ DATA_ROOT_PATH;
	QDir* dir = new QDir;
	if (!dir->exists(rootPath + CONFIG_PATH))
	{
		dir->mkpath(rootPath + CONFIG_PATH);
	}
	if (!dir->exists(rootPath + RECORD_PATH))
	{
		dir->mkpath(rootPath + RECORD_PATH);
	}
	if (!dir->exists(rootPath + AVATAR_PATH))
	{
		dir->mkpath(rootPath + AVATAR_PATH);
	}
	if (!dir->exists(rootPath + TMP_PATH))
	{
		dir->mkpath(rootPath + TMP_PATH);
	}
	if (!dir->exists(rootPath + LOG_PATH))
	{
		dir->mkpath(rootPath + LOG_PATH);
	}

	XmppClient* xmpp_client = new XmppClient();

	QString logPath = rootPath + LOG_PATH + LOG_FILE_NAME;
	if (!STLogger::instance().open_log(reinterpret_cast<const wchar_t *>(logPath.utf16())))
	{
		std::cout << "Log::open_log() failed" << std::endl;
		return false;
	}
	QApplication a(argc, argv);
	//STSample w;
	//w.show();
	STLoginRotate loginRotateWindow(xmpp_client);
	loginRotateWindow.show();
	return a.exec();
}
