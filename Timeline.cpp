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
	static int offset = -12;

	if(offset < 0)
		offset = style()->pixelMetric(QStyle::PM_SliderLength);



	painter.begin(this);
	int current_position = 0;
	qreal alpha = 0;
	bool has_errors = false;
	while(version <= maxVersion) {
		timestamp = rpldata_wsn_version_get_timestamp(version);
		int position = QStyle::sliderPositionFromValue(min, max, ceil(timestamp*100), w-offset) + offset/2;
		if ( position == current_position ) {
		    alpha = 64 + (255-64) * alpha / 255;
		} else {
		    if ( has_errors ) {
	            painter.setPen(QColor(255, 0, 0, 255));
		    } else {
              painter.setPen(QColor(0, 0, 0, alpha));
		    }
	        painter.drawLine(current_position, 0, current_position, h);
	        alpha = 64;
	        has_errors = false;
	        current_position = position;
		}
        has_errors = has_errors || rpldata_wsn_version_get_has_errors(version);
        if ( rpldata_wsn_version_get_has_errors(version) ) {
          //printf("e: %d (%f)\n", version, timestamp);
        }
		version++;
	}
    if ( has_errors ) {
        painter.setPen(QColor(255, 0, 0, alpha));
    } else {
      painter.setPen(QColor(0, 0, 0, alpha));
    }
    painter.drawLine(current_position, 0, current_position, h);
	painter.end();

	QSlider::paintEvent(ev);
}
