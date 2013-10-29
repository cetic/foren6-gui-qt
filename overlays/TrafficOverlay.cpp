#include "TrafficOverlay.h"
#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include <QApplication>

TrafficOverlay::TrafficOverlay()
{
	max_packets_encountered = 0;
}

bool TrafficOverlay::nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush) {
	if(!newPen || !newBrush)
		return false;

    int packet_count = node_get_sixlowpan_statistics(node->getNodeData())->packet_count;
	if(packet_count > max_packets_encountered) {
		max_packets_encountered = packet_count;
		//invalidate();
	}

	if(node->isSelected())
		*newPen = QPen(QColor(Qt::darkBlue));
	else {
		int color = 240 - qMin(packet_count*240/max_packets_encountered, 240);
		*newPen = QPen(QColor(color, color, color));
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

bool TrafficOverlay::nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor) {
	Q_UNUSED(node);
	Q_UNUSED(newColor);

	if(newFont) {
		*newFont = QApplication::font();
		newFont->setPointSize(8);
	}

	return true;
}

bool TrafficOverlay::linkPen(rpl::Link *link, QPen *newPen) {
	Q_UNUSED(link);

	*newPen = QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	return true;
}

bool TrafficOverlay::nodeInfoText(rpl::Node *node, QString *  infoText) {
    int packet_count = node_get_sixlowpan_statistics(node->getNodeData())->packet_count;
    if (infoText) {
        *infoText = QString::number(packet_count);
    }
    return true;
}
