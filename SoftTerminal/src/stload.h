#ifndef _STLOAD_H_
#define _STLOAD_H_

#include <QWidget>
#include <QMovie>
#include "ui_STLoad.h"

namespace tahiti
{

	class STLoad : public QWidget
	{
		Q_OBJECT

	public:
		STLoad(QWidget * parent = 0);
		~STLoad();

	private:
		Ui::STLoadClass ui;
		QMovie* m_movie;
	};
}
#endif
