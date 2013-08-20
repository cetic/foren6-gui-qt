#include "DodagOverlay.h"
#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include <QApplication>
#include <QCryptographicHash>

QColor DodagOverlay::dodagToColor(const void *dodagid, int16_t version) {
	unsigned char color[3] = {0};
	char rawDodagId[18];

	qMemCopy(rawDodagId, dodagid, 16);
	rawDodagId[16] = version & 0xFF;
	rawDodagId[17] = (version >> 8) & 0xFF;

	QByteArray dodagHash = QCryptographicHash::hash(QByteArray(rawDodagId, 18), QCryptographicHash::Md5);

	for(int i = 0; i < dodagHash.size(); i++) {
		color[i%3] += dodagHash.at(i);
	}

	return QColor(color[0]/2, color[1]/2, color[2]/2);
}

bool DodagOverlay::nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush) {
	if(!newPen || !newBrush)
		return false;

	const di_dodag_ref_t* dodagRef = node_get_dodag(node->getNodeData());
	QColor dodagColor;

	if(dodagRef)
		dodagColor = dodagToColor(&dodagRef->dodagid, dodagRef->version);
	else dodagColor = QColor(Qt::black);

	if(node->isSelected())
		*newPen = QPen(dodagColor.lighter());
	else
		*newPen = QPen(dodagColor);

	if(node_get_rank(node->getNodeData()) == 256) {
		newPen->setWidth(2);
	} else {
		newPen->setWidth(1);
	}

	newBrush->setStyle(Qt::SolidPattern);
	newBrush->setColor(Qt::white);

	return true;
}

bool DodagOverlay::nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor) {
	Q_UNUSED(node);
	Q_UNUSED(newColor);

	if(newFont) {
		*newFont = QApplication::font();
		newFont->setPointSize(8);
	}

	return true;
}

bool DodagOverlay::linkPen(rpl::Link *link, QPen *newPen) {
	Q_UNUSED(link);

	*newPen = QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	return true;
}
