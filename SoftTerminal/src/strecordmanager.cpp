#include "strecordmanager.h"

using namespace tahiti;

STRecordManager::STRecordManager(QString otherJid)
{
	QString path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
		+ DATA_ROOT_PATH + RECORD_PATH + otherJid + RECORD_FILE_TYPE;
	m_recordFilePath = path;
	m_recordFile = new QFile(path);
}

STRecordManager::~STRecordManager()
{
	delete(m_recordFile);
}

void STRecordManager::removeRecord()
{
	QFile::remove(m_recordFilePath);
}

bool STRecordManager::isRecordExist()
{
	return QFile::exists(m_recordFilePath);
}

void STRecordManager::writeRecordItem(RecordItem item)
{
	if (!item.jid.isEmpty())
	{
		m_recordFile->open(QIODevice::WriteOnly);
	}
	else
	{
		m_recordFile->open(QIODevice::WriteOnly | QIODevice::Append);
	}

	if (item.time.size() > 0)
	{
		QDataStream out(m_recordFile);
		out << item.time << (qint32)item.from << item.jid
			<< (qint32)item.type << item.content;
	}
	m_recordFile->close();
}

QList<RecordItem> STRecordManager::getRecordItemList()
{
	m_recordFile->open(QIODevice::ReadOnly);
	QList<RecordItem> list;
	QDataStream readDataStream(m_recordFile);
	QString lineStr;
	while (!readDataStream.atEnd())
	{
		RecordItem recordItem;
		qint32 from;
		qint32 type;
		readDataStream >> recordItem.time >> from >> recordItem.jid
			>> type >> recordItem.content;
		recordItem.from = (MessageFrom)from;
		recordItem.type = (MessageType)type;
		list.append(recordItem);
	}
	m_recordFile->close();
	return list;
}

RecordItem STRecordManager::getLastestRecordItem()
{
	QList<RecordItem> list = getRecordItemList();
	if (list.size() == 0)
	{
		RecordItem blankRecordItem;
		return blankRecordItem;
	}
	int lastIndex = list.size() - 1;
	return list.at(lastIndex);
}
