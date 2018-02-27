#include "stconfig.h"

using namespace tahiti;

STConfig::STConfig()
{
}

STConfig::~STConfig()
{

}

QString STConfig::getConfig(QString key)
{
	QString path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
		+ DATA_ROOT_PATH + CONFIG_PATH + CONFIG_FILE_NAME;
	QSettings* config = new QSettings(path, QSettings::IniFormat);
	config->setIniCodec("UTF8");
	QString value = config->value(key).toString();
	delete config;
	return value;
}

void STConfig::setConfig(QString key, QString value)
{
	QString path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
		+ DATA_ROOT_PATH + CONFIG_PATH + CONFIG_FILE_NAME;
	QSettings* config = new QSettings(path, QSettings::IniFormat);
	config->setIniCodec("UTF8");
	config->setValue(key, value);
	delete config;
}
