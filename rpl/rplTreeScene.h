#pragma once

#include <QGraphicsScene>
#include <QModelIndex>
#include <QVariant>
#include <rpl_packet_parser.h>
#include <QTimer>
#include <QSettings>

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

		void setBackground(QString newBackgroundFile);
		void setLayout(QSettings *  layout);
        void getLayout(QSettings *  layout);
		void clearLayout();

		Node *getNode(addr_wpan_t address);
		Link *getLink(addr_wpan_t child, addr_wpan_t parent);

		const QHash<addr_wpan_t, Node*>& getNodes() { return _nodes; }
		const QHash<QPair<addr_wpan_t, addr_wpan_t>, Link*>& getLinks() { return _links; }


        protected:
		void drawBackground( QPainter * painter, const QRectF & rect );

	protected slots:
		void updateNodePositions();
		void toggleNodeMovement();
        void toggleNodeInfo();

	private:
		QTimer _updateDagsTimer;
		QHash<addr_wpan_t, Node*> _nodes;
		QHash<QPair<addr_wpan_t, addr_wpan_t>, Link*> _links;
		QSettings * layout;
        QPixmap * background;
        QString backgroundFile;
        qreal scale;
        bool showNodeInfo;
};

}
