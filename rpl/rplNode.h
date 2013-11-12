/*
 * Copyright (c) 2013, CETIC.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file
 *         RPL node paint, physics engine & user control
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#ifndef RPL_NODE_H
#define RPL_NODE_H

#include <QGraphicsItem>
#include <data_info/node.h>
#include <QElapsedTimer>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QColor>

namespace rpl
{

class Link;
class NetworkInfoManager;

class Node : public QGraphicsItemGroup
{
public:
	Node(NetworkInfoManager *networkInfoManager, di_node_t *nodeData, int version);
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
	void setNodeData(di_node_t *data, int version);
	QList<Link*> links() { return _links; }
	int getVersion() { return _version; }

	void setPen(QPen pen) { _ellipse.setPen(pen); }
	void setBrush(QBrush brush) { _ellipse.setBrush(brush); }
	void setFont(QFont font) { _label.setFont(font); _infoLabel.setFont(font); }
	void setTextColor(QColor color);
	void setInfoText(QString infoText);
	void showInfoText(bool show);

	bool isSelected() { return _isSelected; }
	void setSelected(bool selected) { _isSelected = selected; }

	void setName(QString const & name);
	QString getName() const { return _friendlyName; }
	void setLocked(bool locked) { _pinned = locked; }
	bool isLocked() const { return _pinned; }

public slots:
	void updatePosition();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	void alignLabels();
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
	NetworkInfoManager *_networkInfoManager;
	QElapsedTimer _timeElapsedMouseMove;    //Since last pos using mouse
	di_node_t *_nodeData;
	QGraphicsEllipseItem _ellipse;
	QGraphicsSimpleTextItem _label;
    QGraphicsSimpleTextItem _infoLabel;
	qreal _dx, _dy;
	QList<Link*> _links;
	qreal _maxSize;

	QString _friendlyName;
	bool _isBeingMoved;
	bool _pinned;
	bool _isSelected;
	bool _showInfoText;

	//debug
	int _version;  //the version of the node data
	char guard[5];
};

}

#endif // RPL_NODE_H
