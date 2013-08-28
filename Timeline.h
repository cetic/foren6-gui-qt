#ifndef TIMELINE_H
#define TIMELINE_H

#include <QSlider>

class Timeline : public QSlider
{
	Q_OBJECT
public:
	explicit Timeline(QWidget *parent = 0);


protected:
	void paintEvent(QPaintEvent *ev);
	
};

#endif // TIMELINE_H
