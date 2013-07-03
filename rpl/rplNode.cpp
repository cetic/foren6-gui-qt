#include "rplNode.h"
#include <QPointF>
#include <QBrush>
#include "rplLink.h"

namespace rpl
{

Node::Node(di_node_t *nodeData, QString label)
	: _nodeData(nodeData),
	  _ellipse(this),
	  _label(this),
	  _dx(0),
	  _dy(0)
{
	static int ugly_hack = 0;
	setFlags( QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsMovable );
	setAcceptHoverEvents( true );

	_label.setPlainText(QString::number(label.right(2).toInt(0, 16)));
	ugly_hack++;
	this->addToGroup(&_label);

	qreal maxSize = qMax(_label.boundingRect().width(), _label.boundingRect().height()) + 5;

	_ellipse.setRect(0, 0, maxSize, maxSize);
	_ellipse.setBrush(QBrush(Qt::white));
	this->addToGroup(&_ellipse);

	_label.setPos(maxSize/2 - _label.boundingRect().width()/2, maxSize/2 - _label.boundingRect().height()/2);

	setCenterPos(qrand()%500, qrand()%500);
	setZValue(1);
}

Node::~Node() {
	Link *link;

	foreach(link, _links) {
		delete link;
	}

	_links.clear();
}

void Node::setCenterPos(QPointF newpos) {
	setCenterPos(newpos.x(), newpos.y());
}

void Node::setCenterPos(qreal x, qreal y) {
	setPos(x - boundingRect().width()/2, y - boundingRect().height()/2);
}

void Node::setPos(qreal x, qreal y) {
	Link *link;

	QGraphicsItemGroup::setPos(x, y);

	foreach(link, _links) {
		link->updatePosition();
	}
}

void Node::onNodeChanged() {

}

QPointF Node::centerPos() const {
	return QPointF(x() + boundingRect().width()/2, y() + boundingRect().height()/2);
}

void Node::incSpeed(qreal x, qreal y) {
	_dx += x;
	_dy += y;
//	if(_dx > 20)
//		_dx = 20;
//	if(_dx < -20)
//		_dx = -20;

//	if(_dy > 20)
//		_dy = 20;
//	if(_dy < -20)
//		_dy = -20;
}

void Node::updatePosition() {
	if(!_timeElapsed.isValid()) {
		_timeElapsed.start();
	} else {
		qint64 interval = _timeElapsed.restart();
		qreal newX = centerX() + _dx*interval/1000;
		qreal newY = centerY() + _dy*interval/1000;
		_dx *= 0.9;
		_dy *= 0.9;

		if(newX < 0) {
			newX = 0;
			_dx = -_dx/2;
		}
		if(newY < 0) {
			newY = 0;
			_dy = - _dy/2;
		}
		if(newX > 500) {
			newX = 500;
			_dx = -_dx/2;
		}
		if(newY > 500) {
			newY = 500;
			_dy = - _dy/2;
		}
		setCenterPos(newX, newY);
	}
}

}
