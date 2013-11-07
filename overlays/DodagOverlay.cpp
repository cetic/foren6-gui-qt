#include "DodagOverlay.h"
#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include <QApplication>
#include <QCryptographicHash>
#include <stdlib.h>

QColor DodagOverlay::dodagToColor(const void *dodagid, int16_t version) {
	unsigned char color[3] = {0};
	char rawDodagId[18];

	memcpy(rawDodagId, dodagid, 16);
	rawDodagId[16] = version & 0xFF;
	rawDodagId[17] = (version >> 8) & 0xFF;

	QByteArray dodagHash = QCryptographicHash::hash(QByteArray(rawDodagId, 18), QCryptographicHash::Sha1);

	for(int i = 0; i < dodagHash.size(); i++) {
		color[i%3] = color[i%3] + dodagHash.at(i) + color[i%3]*dodagHash.at(i);
	}

	color[0] = qMax(-128, qMin(128, (int)((((int)color[0])-128)*1.2))) + 128;
	color[1] = qMax(-128, qMin(128, (int)((((int)color[1])-128)*1.2))) + 128;
	color[2] = qMax(-128, qMin(128, (int)((((int)color[2])-128)*1.2))) + 128;

	return QColor(color[0], color[1], color[2] );
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

bool DodagOverlay::nodeInfoText(rpl::Node *node, QString *  infoText) {
  if (infoText) {
      const rpl_instance_config_t *config = node_get_instance_config(node->getNodeData());
      if ( config && config->has_dio_config ) {
          *infoText = QString::number(config->version_number);
      } else {
          *infoText = "-";
      }
  }
  return true;
}
