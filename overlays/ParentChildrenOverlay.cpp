#include "ParentChildrenOverlay.h"
#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include "rpl/rplNetworkInfoManager.h"
#include "data_info/hash_container.h"
#include "data_info/link.h"
#include <QApplication>

ParentChildrenOverlay::ParentChildrenOverlay()
{
}

bool ParentChildrenOverlay::nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush) {
	bool makeGray = true;

	if(node->isSelected())
		*newPen = QPen(QColor(Qt::darkBlue));
	else
		*newPen = QPen(QColor(Qt::red));

	if(node_get_rank(node->getNodeData()) == 256) {
		newPen->setWidth(3);
	} else {
		newPen->setWidth(2);
	}

	newBrush->setStyle(Qt::SolidPattern);
	newBrush->setColor(Qt::white);

	if(node->isSelected() == false) {
		rpl::Link *link;
		rpl::TreeScene *scene = dynamic_cast<rpl::TreeScene *>(node->scene());

		if(scene == 0) {
			qDebug("ERROR: SCENE IS NOT A rpl::TreeScene !");
			return false;
		}

		foreach(link, scene->getLinks()) {
			rpl::Node *from, *to;

			from = link->from();
			to = link->to();

			if((from == node && to->isSelected()) || (to == node && from->isSelected()))
				makeGray = false;
		}
	} else makeGray = false;

	if(makeGray) {
		newPen->setColor(Qt::gray);
	} else if(node->isSelected() == false) {
		newPen->setColor(Qt::black);
	}

	return true;
}

bool ParentChildrenOverlay::linkPen(rpl::Link *link, QPen *newPen) {
	Q_UNUSED(link);
	rpl::Node *to, *from;

	from = link->from();
	to = link->to();

	if((to && to->isSelected()) || (from && from->isSelected()))
		*newPen = QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	else
		*newPen = QPen(Qt::gray, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

	return true;
}
