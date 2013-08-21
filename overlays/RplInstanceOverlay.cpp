#include "RplInstanceOverlay.h"
#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include "rpl/rplNetworkInfoManager.h"
#include <QApplication>
#include <QCryptographicHash>

QColor RplInstanceOverlay::rplInstanceToColor(int16_t instance) {
	unsigned char color[3] = {0};

	QByteArray md5Hash = QCryptographicHash::hash(QByteArray((const char*)&instance, sizeof(int16_t)), QCryptographicHash::Md5);

	for(int i = 0; i < md5Hash.size(); i++) {
		color[i%3] += md5Hash.at(i);
	}

	return QColor(color[0]/2+64, color[1]/2+64, color[2]/2+64);
}

bool RplInstanceOverlay::nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush) {
	if(!newPen || !newBrush)
		return false;

	const di_dodag_ref_t* dodagRef = node_get_dodag(node->getNodeData());
	QColor dodagColor;

	if(dodagRef) {
		const di_dodag_t* dodag = rpl::NetworkInfoManager::getInstance()->getDodag(dodagRef);
		dodagColor = rplInstanceToColor(dodag_get_rpl_instance(dodag)->rpl_instance);
	} else dodagColor = QColor(Qt::black);

	if(node->isSelected())
		*newPen = QPen(QColor(Qt::darkBlue));
	else
		*newPen = QPen(dodagColor);

	if(node_get_rank(node->getNodeData()) == 256) {
		newPen->setWidth(3);
	} else {
		newPen->setWidth(2);
	}

	newBrush->setStyle(Qt::SolidPattern);
	newBrush->setColor(Qt::white);

	return true;
}

bool RplInstanceOverlay::nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor) {
	Q_UNUSED(node);
	Q_UNUSED(newColor);

	if(newFont) {
		*newFont = QApplication::font();
		newFont->setPointSize(8);
	}

	return true;
}

bool RplInstanceOverlay::linkPen(rpl::Link *link, QPen *newPen) {
	Q_UNUSED(link);

	*newPen = QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	return true;
}

