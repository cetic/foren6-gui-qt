#include "rplNode.h"
#include <QPointF>
#include <QBrush>
#include "rplLink.h"
#include <QGraphicsSceneMouseEvent>
#include "rplNetworkInfoManager.h"
#include <QApplication>

namespace rpl
{

Node::Node(NetworkInfoManager *networkInfoManager, di_node_t *nodeData)
	: _networkInfoManager(networkInfoManager),
	  _ellipse(this),
	  _label(this),
	  _dx(0),
	  _dy(0),
	  _isBeingMoved(false),
	  _pinned(false),
	  _isSelected(false)
{
	setFlags( QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsMovable);
	setAcceptHoverEvents( true );
	setNodeData(nodeData);

	_label.setPlainText(QString::number((node_get_key(nodeData)->ref.wpan_address & 0xFF), 16));
	this->addToGroup(&_label);

	qreal maxSize = qMax(_label.boundingRect().width(), _label.boundingRect().height()) + 2;

	_ellipse.setRect(0, 0, maxSize, maxSize);
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

QPointF Node::centerPos() const {
	return QPointF(x() + boundingRect().width()/2, y() + boundingRect().height()/2);
}

void Node::incSpeed(qreal x, qreal y) {
	if(_isBeingMoved == false && _pinned == false) {
		_dx += x;
		_dy += y;
	}
}

void Node::updatePosition() {
	if(_isBeingMoved == false && _pinned == false) {
		qint64 interval = 40;
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

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	QGraphicsItemGroup::mousePressEvent(event);
	if(event->button() == Qt::LeftButton) {
		_timeElapsedMouseMove.invalidate();
		_isBeingMoved = true;
	} else if(event->button() == Qt::RightButton) {
		_pinned = !_pinned;
	}
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
	QGraphicsItemGroup::mouseReleaseEvent(event);
	if(event->button() == Qt::LeftButton) {
		if((event->buttonDownScenePos(Qt::LeftButton) - event->scenePos()).manhattanLength() < 4) {
			_networkInfoManager->selectNode(this);
		} else {
			if(_timeElapsedMouseMove.isValid()) {
				qint64 elapsedTime = _timeElapsedMouseMove.restart();
				if(elapsedTime) {
					_dx = (event->scenePos().x() - event->lastScenePos().x()) * 1000 / elapsedTime;
					_dy = (event->scenePos().y() - event->lastScenePos().y()) * 1000 / elapsedTime;
				}
			}
		}

		_isBeingMoved = false;
	}
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
	if(_isBeingMoved) {
		Link *link;
		if(_timeElapsedMouseMove.isValid() == false)
			_timeElapsedMouseMove.start();
		else {
			qint64 elapsedTime = _timeElapsedMouseMove.restart();
			if(elapsedTime) {
				_dx = (event->scenePos().x() - event->lastScenePos().x()) * 1000 / elapsedTime;
				_dy = (event->scenePos().y() - event->lastScenePos().y()) * 1000 / elapsedTime;
			}
		}

		QGraphicsItemGroup::mouseMoveEvent(event);

		foreach(link, _links) {
			link->updatePosition();
		}
	}
}

void Node::setNodeData(di_node_t *data) {
	_nodeData = data;
	if(data)
		node_set_user_data(data, this);
}

}
