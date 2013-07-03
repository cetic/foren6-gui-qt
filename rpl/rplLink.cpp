#include "rplLink.h"
#include "rplNode.h"
#include <QPainter>
#include <math.h>
#include <QGraphicsScene>

namespace rpl
{

static const qreal Pi = 3.14159;


Link::Link(di_link_t *link) : _link(link)
{
	_from = static_cast<Node*>(link->key.child->user_data);
	_to = static_cast<Node*>(link->key.parent->user_data);
	Q_ASSERT(_from);
	Q_ASSERT(_to);
	Q_ASSERT(_from->getNodeData() == link->key.child);
	Q_ASSERT(_to->getNodeData() == link->key.parent);

	_from->addLink(this);
	_to->addLink(this);

	setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

	updatePosition();
}

Link::~Link()
{
	_to->removeLink(this);
	_from->removeLink(this);
}

void Link::updatePosition()
{
	QLineF line(_to->centerPos(), _from->centerPos());
	setLine(line);
}

QRectF Link::boundingRect() const
{
	qreal extra = (pen().width() + 20) / 2.0;

	return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
									  line().p2().y() - line().p1().y()))
			.normalized()
			.adjusted(-extra, -extra, extra, extra);
}

QPainterPath Link::shape() const
{
	QPainterPath path = QGraphicsLineItem::shape();
	path.addPolygon(_arrowHead);
	return path;
}

void Link::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (_from->collidesWithItem(_to))
		return;

	qreal arrowSize = 10;
	painter->setPen(pen());
	painter->setBrush(pen().color());


	QLineF currentLine(_from->centerPos(), _to->centerPos());
	qreal nodeRadius = _to->radius();

	QPointF intersectPoint((1 - nodeRadius / currentLine.length())*currentLine.dx() + currentLine.p1().x(),
						   (1 - nodeRadius / currentLine.length())*currentLine.dy() + currentLine.p1().y());

	QLineF centerLine(intersectPoint, _from->centerPos());


	double angle = ::acos(centerLine.dx() / centerLine.length());
	if (centerLine.dy() >= 0)
		angle = (Pi * 2) - angle;

	QPointF arrowP1 = centerLine.p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
											cos(angle + Pi / 3) * arrowSize);
	QPointF arrowP2 = centerLine.p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
											cos(angle + Pi - Pi / 3) * arrowSize);

	_arrowHead.clear();
	_arrowHead << centerLine.p1() << arrowP1 << arrowP2;

	painter->drawLine(centerLine);
	painter->drawPolygon(_arrowHead);
	if (isSelected()) {
		painter->setPen(QPen(pen().color(), 1, Qt::DashLine));
		QLineF myLine = line();
		myLine.translate(0, 4.0);
		painter->drawLine(myLine);
		myLine.translate(0,-8.0);
		painter->drawLine(myLine);
	}
}

}
