#pragma once

#include <QGraphicsScene>
#include <QModelIndex>
#include <QVariant>
#include <rpl_packet_parser.h>
#include <QTimer>

namespace rpl
{

class Node;
class Link;

class TreeScene : public QGraphicsScene
{
	Q_OBJECT
	public:
		TreeScene();

		void addNode(Node *node);
		void addLink(Link *link);
		void removeNode(Node *node);
		void removeLink(Link *link);
		void clear();

	protected slots:
		void updateNodePositions();


	private:
		QTimer _updateDagsTimer;
		QList<Node*> _nodes;
};

}
