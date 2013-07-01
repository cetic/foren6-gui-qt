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
	_nodes.append(node);
	addItem(node);
}

void TreeScene::addLink(Link *link) {
	_links.append(link);
	addItem(link);
}

void TreeScene::removeNode(Node *node) {
	removeItem(node);
	_nodes.removeAll(node);
}

void TreeScene::removeLink(Link *link) {
	removeItem(link);
	_links.removeAll(link);
}

void TreeScene::updateNodePositions() {
	Link *currentLink;
	Node *n1, *n2;

	/*foreach(currentLink, _links) {
		QPointF pos1 = currentLink->from()->centerPos();
		QPointF pos2 = currentLink->to()->centerPos();
		qreal vx = pos2.x() - pos1.x();
		qreal vy = pos2.y() - pos1.y();
		qreal dist = sqrt(vx * vx + vy * vy);
		if(qAbs(dist) < 0.01) dist = 0.01;
		qreal factor = (1 - dist)/(dist * 3);
		currentLink->from()->incSpeed(-factor * vx, -factor * vy);
		currentLink->to()->incSpeed(factor * vx, factor * vy);
	}*/

	foreach(n1, _nodes) {
		qreal dx = 0, dy = 0;
		foreach(n2, _nodes) {
			if(n1 == n2)
				continue;
			QPointF pos1 = n1->centerPos();
			QPointF pos2 = n2->centerPos();
			qreal vx = pos2.x() - pos1.x();
			qreal vy = pos2.y() - pos1.y();
			qreal dist = vx * vx + vy * vy;
			if(dist == 0) {
				dx += (qrand() * 5.0) / RAND_MAX;
				dy += (qrand() * 5.0) / RAND_MAX;
			} else if(dist < 100 * 100) {
				dx += vx / dist;
				dy += vy / dist;
			}
		}
		qreal dist = dx * dx + dy * dy;
		if (dist > 0) {
			dist = sqrt(dist) / 2;
			n1->incSpeed(-dx *100 / dist, -dy *100  / dist);
			n1->updatePosition();
		}
	}

	foreach(currentLink, _links) {
		currentLink->update();
	}
}

}
