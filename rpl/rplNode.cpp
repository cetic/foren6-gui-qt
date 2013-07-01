#include "rplNode.h"
#include <QPointF>

namespace rpl
{

Node::Node(di_node_t *nodeData, QString label)
	: _nodeData(nodeData),
	  _ellipse(0, 0, 100, 10, this),
	  _label(label, this),
	  _dx(0),
	  _dy(0)
{
	this->addToGroup(&_ellipse);
	this->addToGroup(&_label);
	setFlags( QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsMovable );
	setAcceptHoverEvents( true );
	this->setToolTip("A test");
}

void Node::setCenterPos(QPointF newpos) {
	setCenterPos(newpos.x(), newpos.y());
}

void Node::setCenterPos(qreal x, qreal y) {
	setPos(x - boundingRect().width()/2, y - boundingRect().height()/2);
	_label.setPos(boundingRect().width()/2 - _label.boundingRect().width()/2, boundingRect().height()/2 - _label.boundingRect().height()/2);
}

void Node::onNodeChanged() {

}

QPointF Node::centerPos() const {
	return QPointF(x() + boundingRect().width()/2, y() + boundingRect().height()/2);
}

void Node::incSpeed(qreal x, qreal y) {
	_dx += x;
	_dy += y;
	if(_dx > 10)
		_dx = 10;
	if(_dx < -10)
		_dx = -10;

	if(_dy > 10)
		_dy = 10;
	if(_dy < -10)
		_dy = -10;
}

void Node::updatePosition() {
	if(!_timeElapsed.isValid()) {
		_timeElapsed.start();
	} else {
		qint64 interval = _timeElapsed.restart();
		qreal newX = centerX() + _dx*interval/1000;
		qreal newY = centerY() + _dy*interval/1000;
		_dx /= 2;
		_dy /= 2;
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
