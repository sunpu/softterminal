#ifndef _STEMOTION_H
#define _STEMOTION_H

#include <QWidget>
#include <QLabel>
#include <QString>
#include <QTextCodec>
#include <windows.h>
#include <Winuser.h>
#include "stcommon.h"
#include "ui_STEmotion.h"

using namespace std;

namespace tahiti
{
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

	// ------------------------ STEmotion ------------------------
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
