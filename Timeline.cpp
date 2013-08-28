#include "Timeline.h"
#include "data_info/rpl_data.h"
#include <math.h>
#include <QPainter>
#include <QStyle>

Timeline::Timeline(QWidget *parent) :
    QSlider(parent)
{
}


void Timeline::paintEvent(QPaintEvent *ev) {
	QPainter painter;
	int max = maximum();
	int min = minimum();
	int w = width();
	int h = height();
	int maxVersion = rpldata_get_wsn_last_version();
	int version = 1;
	double timestamp;
	static int offset = -1;

	if(offset == -1)
		offset = style()->pixelMetric(QStyle::PM_SliderLength);



	painter.begin(this);
	painter.setPen(QColor(0, 0, 0, 64));
	while(version <= maxVersion && (timestamp = rpldata_wsn_version_get_timestamp(version)) < max) {

		int position = QStyle::sliderPositionFromValue(min, max, ceil(timestamp*100), w-offset) + offset/2;
		painter.drawLine(position, 0, position, h);
		version++;
	}
	painter.end();

	QSlider::paintEvent(ev);
}
