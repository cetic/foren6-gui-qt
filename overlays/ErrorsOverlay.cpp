#include "ErrorsOverlay.h"

#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include <QApplication>

ErrorsOverlay::ErrorsOverlay() {
	maxErrorCount = 0;
}

bool ErrorsOverlay::nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush) {
	if(!newPen || !newBrush)
		return false;

	if(!node->getNodeData())
		return false;

	int errorCount = getErrorCount(node);

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

bool ErrorsOverlay::nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor) {
	Q_UNUSED(node);
	Q_UNUSED(newColor);

	if(newFont) {
		*newFont = QApplication::font();
		newFont->setPointSize(8);
	}

	return true;
}

bool ErrorsOverlay::linkPen(rpl::Link *link, QPen *newPen) {
	Q_UNUSED(link);

	*newPen = QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	return true;
}

bool ErrorsOverlay::nodeInfoText(rpl::Node *node, QString *  infoText) {
  return false;
}
