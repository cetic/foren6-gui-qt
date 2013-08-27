#pragma once

#include <QGraphicsScene>
#include <QModelIndex>
#include <QVariant>
#include <rpl_packet_parser.h>
#include <QTimer>

class IOverlayModel;

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
		void removeAllLinks();
		void removeAllNodes();
		void clear();

	protected slots:
		void updateNodePositions();
		void toggleNodeMovement();


	private:
		QTimer _updateDagsTimer;
		QList<Node*> _nodes;
		QList<Link*> _links;
};

}
