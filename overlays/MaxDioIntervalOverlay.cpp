#include "MaxDioIntervalOverlay.h"
#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include <QApplication>

MaxDioIntervalOverlay::MaxDioIntervalOverlay()
{
	max_dio_interval = 0;
}

bool MaxDioIntervalOverlay::nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush) {
	if(!newPen || !newBrush)
		return false;

    double interval = node_get_rpl_statistics(node->getNodeData())->max_dio_interval;
	if(interval > max_dio_interval) {
		max_dio_interval = interval;
		//invalidate();
	}

	if(node->isSelected())
		*newPen = QPen(QColor(Qt::darkBlue));
	else if(max_dio_interval > 0) {
		int color = qMin(static_cast<int>(interval*240/max_dio_interval), 240);
		*newPen = QPen(QColor(color, color, color));
	} else {
		*newPen = QPen(QColor(Qt::black));
	}

	if(node_get_rank(node->getNodeData()) == 256) {
		newPen->setWidth(3);
	} else {
		newPen->setWidth(2);
	}

	newBrush->setStyle(Qt::SolidPattern);
	newBrush->setColor(Qt::white);

	return true;
}

bool MaxDioIntervalOverlay::nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor) {
	Q_UNUSED(node);
	Q_UNUSED(newColor);

	if(newFont) {
		*newFont = QApplication::font();
		newFont->setPointSize(8);
	}

	return true;
}

bool MaxDioIntervalOverlay::linkPen(rpl::Link *link, QPen *newPen) {
	Q_UNUSED(link);

	*newPen = QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	return true;
}

bool MaxDioIntervalOverlay::nodeInfoText(rpl::Node *node, QString *  infoText) {
    double interval = node_get_rpl_statistics(node->getNodeData())->max_dio_interval;
    if (infoText) {
        *infoText = QString::number(interval, 'f', 0);
    }
    return true;
}
