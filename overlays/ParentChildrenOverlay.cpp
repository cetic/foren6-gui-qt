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
	hash_container_ptr link_container = rpldata_get_links(rpl::NetworkInfoManager::getInstance()->getVersion());
	hash_iterator_ptr it = hash_begin(NULL, NULL), itEnd = hash_begin(NULL, NULL);

	bool makeGray = true;

	if(node->isSelected())
		*newPen = QPen(QColor(Qt::darkBlue));
	else
		*newPen = QPen(QColor(Qt::black));

	if(node_get_rank(node->getNodeData()) == 256) {
		newPen->setWidth(3);
	} else {
		newPen->setWidth(2);
	}

	newBrush->setStyle(Qt::SolidPattern);
	newBrush->setColor(Qt::white);

	if(node->isSelected() == false && link_container) {
		for(hash_begin(link_container, it), hash_end(link_container, itEnd); !hash_it_equ(it, itEnd); hash_it_inc(it)) {
			di_link_t *link = *(di_link_t **)hash_it_value(it);
			rpl::Node *from, *to;

			from = (rpl::Node*) node_get_user_data(rpl::NetworkInfoManager::getInstance()->getNode(&link_get_key(link)->ref.child));
			to =   (rpl::Node*) node_get_user_data(rpl::NetworkInfoManager::getInstance()->getNode(&link_get_key(link)->ref.parent));

			if(!from || !to)
				continue;

			if((from == node && to->isSelected()) || (to == node && from->isSelected()))
				makeGray = false;
		}
	} else makeGray = false;

	if(makeGray) {
		newPen->setColor(Qt::gray);
	}

	hash_it_destroy(it);
	hash_it_destroy(itEnd);

	return true;
}

bool ParentChildrenOverlay::nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor) {
	Q_UNUSED(node);
	Q_UNUSED(newColor);

	if(newFont) {
		*newFont = QApplication::font();
		newFont->setPointSize(8);
	}

	return true;
}

bool ParentChildrenOverlay::linkPen(rpl::Link *link, QPen *newPen) {
	Q_UNUSED(link);
	di_node_t* toNode, *fromNode;
	rpl::Node *to, *from;

	if(link->getLinkData() == 0)
		return false;

	toNode = rpl::NetworkInfoManager::getInstance()->getNode(&link_get_key(link->getLinkData())->ref.parent);
	if(!toNode) {
		qDebug("Wrong link parent: %llx, version %d", link_get_key(link->getLinkData())->ref.parent.wpan_address, link->getVersion());
		return false;
	}
	fromNode = rpl::NetworkInfoManager::getInstance()->getNode(&link_get_key(link->getLinkData())->ref.child);
	if(!fromNode) {
		qDebug("Wrong link parent: %llx, version %d", link_get_key(link->getLinkData())->ref.child.wpan_address, link->getVersion());
		return false;
	}
	from = (rpl::Node*) node_get_user_data(fromNode);
	to = (rpl::Node*) node_get_user_data(toNode);
	if((to && to->isSelected()) || (from && from->isSelected()))
		*newPen = QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	else
		*newPen = QPen(Qt::gray, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

	return true;
}
