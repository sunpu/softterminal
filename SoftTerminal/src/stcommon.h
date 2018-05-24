#ifndef _STCOMMON_H_
#define _STCOMMON_H_

#include <QString>
#include <QMetaType>

namespace tahiti
{
#define DATA_ROOT_PATH "/VMediax/SoftTerminal/"
#define CONFIG_PATH "config/"
#define RECORD_PATH "record/"
#define AVATAR_PATH "avatar/"
#define TMP_PATH "tmp/"
#define LOG_PATH "log/"

#define CONFIG_FILE_NAME "softterminal.ini"
#define LOG_FILE_NAME "softterminal.log"

#define RECORD_FILE_TYPE ".dat"

#define SHADOW_WIDTH 10
#define MAX_STREAM_NUM 6

	enum ResizeRegion
	{
		Default,
		North,
		NorthEast,
		East,
		SouthEast,
		South,
		SouthWest,
		West,
		NorthWest
	};

	enum MessageFrom
	{
		Self = 0,
		Other
	};

	enum MessageType
	{
		MT_Text = 0,
		MT_Class
	};

	typedef struct user
	{
		QString jid;
		QString userName;
		QString photoPath;
		QString telephone;
		QString email;
		QString description;
	} UserInfo;

	Q_DECLARE_METATYPE(UserInfo);

	typedef struct group
	{
		QString id;
		QString name;
		QString description;
	} GroupInfo;

	typedef struct recordItem
	{
		QString time;
		MessageFrom from;
		QString jid;
		QString pic;
		MessageType type;
		QString content;
	} RecordItem;

	enum ActionType
	{
		AT_NONE,
		AT_Select,
		AT_Pen,
		AT_Text
	};

	typedef struct fileInfo
	{
		QString type;
		int id;
		QString name;
	} FileInfo;
}
#endif
