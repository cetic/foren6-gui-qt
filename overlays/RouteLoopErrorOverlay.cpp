#include "RouteLoopErrorOverlay.h"
#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include <QApplication>

RouteLoopErrorOverlay::RouteLoopErrorOverlay() {
	maxErrorCount = 0;
}

bool RouteLoopErrorOverlay::nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush) {
	di_node_t *node_data = node->getNodeData();
	if(!newPen || !newBrush)
		return false;

	if(!node_data)
		return false;

	int errorCount = node_get_route_error_count(node_data);

	if(maxErrorCount < errorCount)
		maxErrorCount = errorCount;

	if(node->isSelected())
		*newPen = QPen(QColor(Qt::darkBlue));
	else {
		int color;

		if(maxErrorCount > 0 && errorCount)
			color = qMin(errorCount*192/maxErrorCount, 192) + 63;
		else
			color = 0;

		*newPen = QPen(QColor(color, 0, 0));
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

bool RouteLoopErrorOverlay::nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor) {
	Q_UNUSED(node);
	Q_UNUSED(newColor);

	if(newFont) {
		*newFont = QApplication::font();
		newFont->setPointSize(8);
	}

	return true;
}

bool RouteLoopErrorOverlay::linkPen(rpl::Link *link, QPen *newPen) {
	Q_UNUSED(link);

	*newPen = QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	return true;
}

