#ifndef RPLLINK_H
#define RPLLINK_H

#include <data_info/link.h>
#include <QGraphicsLineItem>
#include <QPen>

namespace rpl
{

class Node;

class Link : public QGraphicsLineItem
{
	public:
		Link(di_link_t *link, int version, Node *from, Node *to);
		virtual ~Link();

		qreal weight() { return 128; } //{ return link_get_metric(_link)->value; }
		Node *to() { return _to; }
		Node *from() { return _from; }
		di_link_t *getLinkData() { return _link; }
		void setLinkData(di_link *link, int version) { _link = link; _version = version; }
		int getVersion() { return _version; }

		void updatePosition();

		virtual QRectF boundingRect() const;
		virtual QPainterPath shape() const;
		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	private:
		di_link_t *_link;
		Node *_from;
		Node *_to;
		QPolygonF _arrowHead;

		//debug
		int _version;  //the version of the link data
};

}

#endif // RPLLINK_H
