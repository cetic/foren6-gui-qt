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

	if(node_get_max_dao_interval(node->getNodeData()) > max_dao_interval) {
		max_dao_interval = node_get_max_dao_interval(node->getNodeData());
		//invalidate();
	}

	if(node->isSelected())
		*newPen = QPen(QColor(Qt::darkBlue));
	else if(max_dao_interval > 0) {
		int color = qMin(static_cast<int>(node_get_max_dao_interval(node->getNodeData())*240/max_dao_interval), 240);
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

bool MaxDaoIntervalOverlay::nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor) {
	Q_UNUSED(node);
	Q_UNUSED(newColor);

	if(newFont) {
		*newFont = QApplication::font();
		newFont->setPointSize(8);
	}

	return true;
}

bool MaxDaoIntervalOverlay::linkPen(rpl::Link *link, QPen *newPen) {
	Q_UNUSED(link);

	*newPen = QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	return true;
}

