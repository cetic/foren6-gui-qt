#include "RankErrorOverlay.h"
#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include <QApplication>

RankErrorOverlay::RankErrorOverlay() {
	maxErrorCount = 0;
}

bool RankErrorOverlay::nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush) {
	di_node_t *node_data = node->getNodeData();
	if(!newPen || !newBrush)
		return false;

	if(!node_data)
		return false;

	int errorCount = node_get_upward_error_count(node->getNodeData()) + node_get_downward_error_count(node->getNodeData());

	if(maxErrorCount < errorCount)
		maxErrorCount = errorCount;

	if(node->isSelected())
		*newPen = QPen(QColor(Qt::darkBlue));
	else {
		int color;

		if(maxErrorCount > 0)
			color = qMin(errorCount*255/maxErrorCount, 255);
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

bool RankErrorOverlay::nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor) {
	Q_UNUSED(node);
	Q_UNUSED(newColor);

	if(newFont) {
		*newFont = QApplication::font();
		newFont->setPointSize(8);
	}

	return true;
}

bool RankErrorOverlay::linkPen(rpl::Link *link, QPen *newPen) {
	Q_UNUSED(link);

	*newPen = QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	return true;
}
