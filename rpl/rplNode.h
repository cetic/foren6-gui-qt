#ifndef RPL_NODE_H
#define RPL_NODE_H

#include <QGraphicsItem>
#include <data_info/node.h>
#include <QElapsedTimer>

namespace rpl
{

class Link;

class Node : public QGraphicsItemGroup
{
	public:
		Node(di_node_t *nodeData, QString label);
		~Node();

		void addLink(Link *link) { _links.append(link); }
		void removeLink(Link *link) { _links.removeOne(link); }

		void setCenterPos(QPointF newpos);
		void setCenterPos(qreal x, qreal y);

		QPointF centerPos() const;
		qreal centerX() const { return centerPos().x(); }
		qreal centerY() const { return centerPos().y(); }

		void setPos(qreal x, qreal y);

		void incSpeed(qreal x, qreal y);
		qreal speedX() { return _dx; }
		qreal speedY() { return _dy; }

		qreal radius() { return _ellipse.rect().width() / 2; }

		di_node_t *getNodeData() { return _nodeData; }
		QList<Link*> links() { return _links; }

	public slots:
		void updatePosition();
		void onNodeChanged();

	private:
		QElapsedTimer _timeElapsed;    //Since last pos update
		di_node_t *_nodeData;
		QGraphicsEllipseItem _ellipse;
		QGraphicsTextItem _label;
		qreal _dx, _dy;
		QList<Link*> _links;
};

}

#endif // RPL_NODE_H
