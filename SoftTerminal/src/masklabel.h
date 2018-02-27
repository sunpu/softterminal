#ifndef _MASKLABEL_H
#define _MASKLABEL_H

#include <QLabel>

class MaskLabel : public QLabel
{
public:
	MaskLabel(QWidget *parent = 0);

protected:
	void paintEvent(QPaintEvent *);
};
#endif
