#include "MaxDaoIntervalOverlay.h"
#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include <QApplication>

MaxDaoIntervalOverlay::MaxDaoIntervalOverlay()
{
	max_dao_interval = 0;
}

bool MaxDaoIntervalOverlay::nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush) {
	if(!newPen || !newBrush)
		return false;

    double interval = node_get_rpl_statistics(node->getNodeData())->max_dao_interval;
	if(interval > max_dao_interval) {
		max_dao_interval = interval;
		//invalidate();
	}

	if(node->isSelected())
		*newPen = QPen(QColor(Qt::darkBlue));
	else if(max_dao_interval > 0) {
		int color = qMin(static_cast<int>(interval*240/max_dao_interval), 240);
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

bool MaxDaoIntervalOverlay::nodeInfoText(rpl::Node *node, QString *  infoText) {
    double interval = node_get_rpl_statistics(node->getNodeData())->max_dao_interval;
    if (infoText) {
        *infoText = QString::number(interval, 'f', 0);
    }
    return true;
}
