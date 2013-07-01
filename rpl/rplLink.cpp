#include "rplLink.h"
#include "rplNode.h"
#include <QPainter>

namespace rpl
{
Link::Link(di_link_t *link) : _link(link)
{
	_from = static_cast<Node*>(link->key.child->user_data);
	_to = static_cast<Node*>(link->key.parent->user_data);
	Q_ASSERT(_from);
	Q_ASSERT(_to);
}

Link::~Link()
{
}

QPen Link::pen() const
{
	return _pen;
}

void Link::setPen(const QPen &pen)
{
	if (_pen == pen)
		return;
	prepareGeometryChange();
	_pen = pen;
	update();
}

QRectF Link::boundingRect() const
{
	if (_pen.widthF() == 0.0) {
		const qreal x1 = _from->centerPos().x();
		const qreal y1 = _from->centerPos().y();
		const qreal x2 = _to->centerPos().x();
		const qreal y2 = _to->centerPos().y();
		qreal lx = qMin(x1, x2);
		qreal rx = qMax(x1, x2);
		qreal ty = qMin(y1, y2);
		qreal by = qMax(y1, y2);
		return QRectF(lx, ty, rx - lx, by - ty);
	}
	return shape().controlPointRect();
}

QPainterPath Link::shape() const
{
	QPainterPath path;
	path.moveTo(_from->centerPos());
	path.lineTo(_to->centerPos());
	const qreal penWidthZero = qreal(0.00000001);

	QPainterPathStroker ps;
	ps.setCapStyle(_pen.capStyle());
	if (_pen.widthF() <= 0.0)
		ps.setWidth(penWidthZero);
	else
		ps.setWidth(_pen.widthF());
	ps.setJoinStyle(_pen.joinStyle());
	ps.setMiterLimit(_pen.miterLimit());
	QPainterPath p = ps.createStroke(path);
	p.addPath(path);
	return p;
}

void Link::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(widget);
	painter->setPen(_pen);
	painter->drawLine(_from->centerPos(), _to->centerPos());
}

}
