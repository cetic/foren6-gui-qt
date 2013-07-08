#include "rplTreeScene.h"
#include "rplNode.h"
#include <rpl_packet_parser.h>
#include <QGraphicsTextItem>
#include <uthash.h>
#include <utlist.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "rplLink.h"
#include <math.h>

#include <QVector2D>

namespace rpl
{

TreeScene::TreeScene()
{
	_updateDagsTimer.setInterval(100);
	_updateDagsTimer.setSingleShot(false);
	connect(&_updateDagsTimer, SIGNAL(timeout()), this, SLOT(updateNodePositions()));
	_updateDagsTimer.start();
}


void TreeScene::addNode(Node *node) {
	qDebug("Adding Node %p %llX", node, node->getNodeData()->wpan_address);
	_nodes.append(node);
	addItem(node);
}

void TreeScene::addLink(Link *link) {
	qDebug("Adding Link %p %llX -> %llX", link, link->getLinkData()->key.child, link->getLinkData()->key.parent);
	addItem(link);
}

void TreeScene::removeNode(Node *node) {
	qDebug("Removing Node %p", node);
	removeItem(node);
	_nodes.removeAll(node);
}

void TreeScene::removeLink(Link *link) {
	qDebug("Removing Link %p, raw link %p", link, link->getLinkData());
	removeItem(link);
	qDebug("Removed Link %p, raw link %p", link, link->getLinkData());
}

void TreeScene::updateNodePositions() {
	Link *currentLink;
	Node *n1, *n2;

	foreach(n1, _nodes) {
		foreach(currentLink, n1->links()) {
			QPointF pos1 = currentLink->from()->centerPos();
			QPointF pos2 = currentLink->to()->centerPos();
			qreal vx = pos2.x() - pos1.x();
			qreal vy = pos2.y() - pos1.y();
			qreal dist = sqrt(vx * vx + vy * vy);
			if(qAbs(dist) < 0.01) dist = 0.01;
			qreal factor = (100 - dist)/(dist * 3);
			currentLink->from()->incSpeed(-factor * vx, -factor * vy);
			currentLink->to()->incSpeed(factor * vx, factor * vy);
		}


		qreal dx = 0, dy = 0;
		foreach(n2, _nodes) {
			if(n1 == n2)
				continue;
			QPointF pos1 = n1->centerPos();
			QPointF pos2 = n2->centerPos();
			qreal vx = pos1.x() - pos2.x();
			qreal vy = pos1.y() - pos2.y();
			qreal dist = vx*vx + vy*vy;

			if(dist == 0) {
				dx += qrand() * 1.0 / RAND_MAX;
				dy += qrand() * 1.0 / RAND_MAX;
			} else if(dist < 100*100) {
				dx += vx / dist;
				dy += vy / dist;
			}
		}
//		qreal vx1 = 0 - n1->centerPos().x();
//		vx1 *= vx1;
//		qreal vy1 = 0 - n1->centerPos().y();
//		vy1 *= vy1;
//		qreal vx2 = 500 - n1->centerPos().x();
//		vx2 *= vx2;
//		qreal vy2 = 500 - n1->centerPos().y();
//		vy2 *= vy2;


//		if(vx1 > 0)
//			dx += 1/vx1;
//		else dx += 1;
//		if(qAbs(vx2) > 0)
//			dx += 1/vx2;
//		else dx += -1;
//		if(qAbs(vy1) > 0)
//			dx += 1/vy1;
//		else dx += 1;
//		if(qAbs(vy2) > 0)
//			dx += 1/vy2;
//		else dx += -1;

//		if(dx == 0 && dy == 0) {
//			dx += (qrand() * 1.0) / RAND_MAX;
//			dy += (qrand() * 1.0) / RAND_MAX;
//		}
		qreal dist = dx*dx + dy*dy;
		if(dist > 0) {
			dist = sqrt(dist) /2;
			n1->incSpeed(10 * dx / dist, 10 * dy / dist);
		}
		n1->incSpeed(0, 1);
		n1->updatePosition();
	}
}

}
