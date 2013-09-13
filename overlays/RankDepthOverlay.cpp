#include "RankDepthOverlay.h"
#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include <QApplication>
#include <math.h>

bool RankDepthOverlay::nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush) {
	if(!newPen || !newBrush)
		return false;

	if(node->isSelected())
		*newPen = QPen(QColor(Qt::darkBlue));
	else {
		int rank = node_get_rank(node->getNodeData());
		int color = qMax(qMin((rank-256)/10, 255), 0);
		if(rank != 65535) {
			if(rank > 1756)
				color = qMin(rank*(255-150)/(10000-1756) + 150, 255);
			*newPen = QPen(QColor(color, color, color));
		} else {
			*newPen = QPen(QColor(128, 128, 128));
			newPen->setStyle(Qt::DotLine);
		}
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

bool RankDepthOverlay::nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor) {
	Q_UNUSED(node);
	Q_UNUSED(newColor);

	if(newFont) {
		*newFont = QApplication::font();
		newFont->setPointSize(8);
	}

	return true;
}

bool RankDepthOverlay::linkPen(rpl::Link *link, QPen *newPen) {
	Q_UNUSED(link);

	*newPen = QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	return true;
}
