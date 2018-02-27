#ifndef _STEMOTION_H
#define _STEMOTION_H

#include <QWidget>
#include <QLabel>
#include <QString>
#include <QTextCodec>
#include <windows.h>
#include <Winuser.h>
#include "ui_STEmotion.h"

using namespace std;

namespace tahiti
{
	const QString EmotionName[40] = {
		"大笑", "高兴", "微笑", "笑哭", "恶魔", "媚眼", "色", "哼哼",
		"不屑", "我汗", "忧郁", "飞吻", "鬼脸", "愤怒", "伤心", "难受",
		"流泪", "心虚", "恐惧", "睡觉", "生病", "心", "心碎", "便便",
		"炸弹", "篮球", "火箭", "揍", "好的", "表扬", "鄙视", "鼓掌",
		"击掌", "肌肉", "双手赞成", "咖啡", "面包", "音乐", "鬼魂", "马桶"
	};

	const QString EmotionCode[40] = {
		"\\ue057", "\\ue056", "\\ue404", "\\ue412", "\\ue11a", "\\ue405", "\\ue106", "\\ue402",
		"\\ue40e", "\\ue108", "\\ue403", "\\ue418", "\\ue105", "\\ue416", "\\ue413", "\\ue406",
		"\\ue411", "\\ue40f", "\\ue107", "\\ue408", "\\ue40c", "\\ue022", "\\ue023", "\\ue05a",
		"\\ue311", "\\ue42a", "\\ue10d", "\\ue00d", "\\ue420", "\\ue00e", "\\ue421", "\\ue41f",
		"\\ue41d", "\\ue14c", "\\ue427", "\\ue045", "\\ue339", "\\ue03e", "\\ue11b", "\\ue140"
	};

	class STEmotionItem : public QLabel
	{
		Q_OBJECT

	public:
		STEmotionItem(QString fileName);
		~STEmotionItem();
	protected:
		void enterEvent(QEvent* event);
		void leaveEvent(QEvent* event);

	private:
	};

	////////////////////////////////////////////////////////////////////////////////////////////////
	class STEmotion : public QWidget
	{
		Q_OBJECT

	public:
		STEmotion(QWidget * parent = 0);
		~STEmotion();
		public Q_SLOTS:
		void emotionSelected(int, int);
	Q_SIGNALS:
		void chooseEmotion(int index);
	protected:
		bool event(QEvent* event);
	private:
		void addEmotionItem(QString fileName, int index);
	private:
		Ui::STEmotionClass ui;
		QList<QString> m_emotionList;
	};
}

#endif
