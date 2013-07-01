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

		void setCenterPos(QPointF newpos);
		void setCenterPos(qreal x, qreal y);

		QPointF centerPos() const;
		qreal centerX() const { return centerPos().x(); }
		qreal centerY() const { return centerPos().y(); }

		void incSpeed(qreal x, qreal y);
		qreal speedX() { return _dx; }
		qreal speedY() { return _dy; }

		di_node_t *getNodeData() { return _nodeData; }

	public slots:
		void updatePosition();
		void onNodeChanged();

	private:
		QElapsedTimer _timeElapsed;    //Since last pos update
		di_node_t *_nodeData;
		QGraphicsEllipseItem _ellipse;
		QGraphicsTextItem _label;
		qreal _dx, _dy;
};

}

#endif // RPL_NODE_H
