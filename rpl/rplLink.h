#ifndef RPLLINK_H
#define RPLLINK_H

#include <data_info/link.h>
#include <QGraphicsLineItem>
#include <QPen>

namespace rpl
{

class Node;

class Link : public QGraphicsItem
{
	public:
		Link(di_link_t *link);
		virtual ~Link();

		qreal weight() { return _link->metric.value; }
		Node *to() { return _to; }
		Node *from() { return _from; }

		QPen pen() const;
		void setPen(const QPen& pen);
		virtual QRectF boundingRect() const;
		virtual QPainterPath shape() const;
		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	private:
		di_link_t *_link;
		Node *_from;
		Node *_to;
		QPen _pen;
};

}

#endif // RPLLINK_H
