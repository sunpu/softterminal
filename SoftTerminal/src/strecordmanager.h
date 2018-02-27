#ifndef _STRECORDMANAGER_H_
#define _STRECORDMANAGER_H_

#include <QFile>
#include <QStandardPaths>
#include <QDataStream>
#include <QDir>
#include <QDirIterator>
#include "stcommon.h"

using namespace tahiti;

namespace tahiti
{
	class STRecordManager
	{

	public:
		STRecordManager(QString otherJid);
		~STRecordManager();
		bool isRecordExist();
		void removeRecord();
		void writeRecordItem(RecordItem item);
		QList<RecordItem> getRecordItemList();
		RecordItem getLastestRecordItem();
	private:
		QString m_recordFilePath;
		QFile* m_recordFile;
	};
}
#endif
