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
#define PATH_SPLIT "/"

#define CONFIG_FILE_NAME "softterminal.ini"
#define LOG_FILE_NAME "softterminal.log"

#define RECORD_FILE_TYPE ".dat"

#define SHADOW_WIDTH 10
#define MAX_STREAM_NUM 6

#define ST_MESSAGE_SERVER "ST_MESSAGE_SERVER"
#define BROKER_PORT 5555
#define PUBLISHER_PORT 5556
#define FILE_SERVER_PORT 10001

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
		MT_CourseCreate,
		MT_CourseDelete
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

	typedef struct groupMsg
	{
		QString jid;
		QString user;
		QString msg;
		QString time;
	} GroupMsg;

	typedef struct personMsg
	{
		QString jid;
		QString msg;
		QString time;
	} PersonMsg;

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

	const QString EmotionName[40] = {
		"´óÐ¦", "¸ßÐË", "Î¢Ð¦", "Ð¦¿Þ", "¶ñÄ§", "ÃÄÑÛ", "É«", "ºßºß",
		"²»Ð¼", "ÎÒº¹", "ÓÇÓô", "·ÉÎÇ", "¹íÁ³", "·ßÅ­", "ÉËÐÄ", "ÄÑÊÜ",
		"Á÷Àá", "ÐÄÐé", "¿Ö¾å", "Ë¯¾õ", "Éú²¡", "ÐÄ", "ÐÄËé", "±ã±ã",
		"Õ¨µ¯", "ÀºÇò", "»ð¼ý", "×á", "ºÃµÄ", "±íÑï", "±ÉÊÓ", "¹ÄÕÆ",
		"»÷ÕÆ", "¼¡Èâ", "Ë«ÊÖÔÞ³É", "¿§·È", "Ãæ°ü", "ÒôÀÖ", "¹í»ê", "ÂíÍ°"
	};

	const QString EmotionCode[40] = {
		"\\ue057", "\\ue056", "\\ue404", "\\ue412", "\\ue11a", "\\ue405", "\\ue106", "\\ue402",
		"\\ue40e", "\\ue108", "\\ue403", "\\ue418", "\\ue105", "\\ue416", "\\ue413", "\\ue406",
		"\\ue411", "\\ue40f", "\\ue107", "\\ue408", "\\ue40c", "\\ue022", "\\ue023", "\\ue05a",
		"\\ue311", "\\ue42a", "\\ue10d", "\\ue00d", "\\ue420", "\\ue00e", "\\ue421", "\\ue41f",
		"\\ue41d", "\\ue14c", "\\ue427", "\\ue045", "\\ue339", "\\ue03e", "\\ue11b", "\\ue140"
	};
}
#endif
