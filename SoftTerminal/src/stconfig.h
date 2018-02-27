#ifndef _STCONFIG_H_
#define _STCONFIG_H_

#include <QSettings>
#include <QStandardPaths>
#include "stcommon.h"

namespace tahiti
{
	class STConfig
	{
	public:
		STConfig();
		~STConfig();
		static QString getConfig(QString key);
		static void setConfig(QString key, QString value);
	private:
		QString m_configPath;
	};
}
#endif
