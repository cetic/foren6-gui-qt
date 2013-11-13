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

#include "rplNode.h"
#include <QPointF>
#include <QBrush>
#include "rplLink.h"
#include <QGraphicsSceneMouseEvent>
#include "rplNetworkInfoManager.h"
#include <QApplication>
#include <QMenu>
#include <QInputDialog>
#include <QGraphicsView>

namespace rpl {

    static qreal const defaultNodeSize = 32;

      Node::Node(NetworkInfoManager * networkInfoManager, di_node_t * nodeData, int version)
    : _networkInfoManager(networkInfoManager),
        _nodeData(0),
        _ellipse(this), _label(this), _infoLabel(this), _dx(0), _dy(0), _isBeingMoved(false), _pinned(false), _isSelected(false), _showInfoText(false) {
        setFlags(QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
        setAcceptHoverEvents(true);
        setNodeData(nodeData, version);

        QSettings settings;
          _maxSize = settings.value("node_size", defaultNodeSize).toFloat();
          _ellipse.setRect(0, 0, _maxSize, _maxSize);
          this->addToGroup(&_ellipse);

        QFont newFont = QApplication::font();
          newFont.setPointSize(8);
          _label.setFont(newFont);
          this->addToGroup(&_label);
          setName("");

          _infoLabel.setFont(newFont);
          _infoLabel.setVisible(_showInfoText);
          this->addToGroup(&_infoLabel);
          setInfoText("");

        if(networkInfoManager->scene()->hasValidBackground()) {
            int dx = (int)(networkInfoManager->scene()->width() - _maxSize / 2);
            int dy = (int)(networkInfoManager->scene()->height() - _maxSize / 2);
              setCenterPos(qrand() % dx, qrand() % dy);
        } else {
            int dx = (int)((networkInfoManager->scene()->width() - (_maxSize / 2)) / 2);
            int dy = (int)((networkInfoManager->scene()->height() - (_maxSize / 2)) / 2);

            setCenterPos(qrand() % (2 * dx) - dx, qrand() % (2 * dy) - dy);
        }

        setZValue(1);

        qstrcpy(guard, "node");
    }

    Node::~Node() {
        Link *link;

        foreach(link, _links) {
            delete link;
        }

        _links.clear();

        qstrcpy(guard, "edon");
        setNodeData(0, -21);
    }

    void Node::alignLabels() {
        qreal y;

        if(_showInfoText && !_infoLabel.text().isEmpty()) {
            y = _maxSize / 2 - _label.boundingRect().height();
        } else {
            y = _maxSize / 2 - _label.boundingRect().height() / 2;
        }
        _label.setPos(_maxSize / 2 - _label.boundingRect().width() / 2, y);
        _infoLabel.setPos(_maxSize / 2 - _infoLabel.boundingRect().width() / 2, _maxSize / 2);
    }

    void Node::setName(QString const &name) {
        _friendlyName = name;
        if(!name.isEmpty()) {
            _label.setText(name);
        } else {
            _label.setText(QString::number((node_get_key(_nodeData)->ref.wpan_address & 0xFF), 16));
        }
        alignLabels();
    }

    void Node::setInfoText(QString infoText) {
        _infoLabel.setText(infoText);
        alignLabels();
    }

    void Node::showInfoText(bool show) {
        _showInfoText = show;
        _infoLabel.setVisible(_showInfoText);
        alignLabels();
    }

    void Node::setTextColor(QColor color) {
        QBrush brush(color);

        _label.setBrush(brush);
        _infoLabel.setBrush(brush);
    }

    void Node::setCenterPos(QPointF newpos) {
        setCenterPos(newpos.x(), newpos.y());
    }

    void Node::setCenterPos(qreal x, qreal y) {
        setPos(x - _maxSize / 2, y - _maxSize / 2);
    }

    void Node::setPos(qreal x, qreal y) {
        Link *link;

        QGraphicsItemGroup::setPos(x, y);

        foreach(link, _links) {
            link->updatePosition();
        }
    }

    QPointF Node::centerPos() const {
        return QPointF(x() + _maxSize / 2, y() + _maxSize / 2);
    } void Node::incSpeed(qreal x, qreal y) {
        if(_isBeingMoved == false && _pinned == false) {
            _dx += x;
            _dy += y;
        }
    }

    void Node::updatePosition() {
        if(_isBeingMoved == false && _pinned == false) {
            QRectF rect = _networkInfoManager->scene()->sceneRect();
            qint64 interval = 40;
            qreal newX = centerX() + _dx * interval / 1000;
            qreal newY = centerY() + _dy * interval / 1000;

            _dx *= 0.9;
            _dy *= 0.9;

            if(newX < rect.left()) {
                newX = rect.left();
                _dx = -_dx / 2;
            }
            if(newY < rect.top()) {
                newY = rect.top();
                _dy = -_dy / 2;
            }
            if(newX > rect.right()) {
                newX = rect.right();
                _dx = -_dx / 2;
            }
            if(newY > rect.bottom()) {
                newY = rect.bottom();
                _dy = -_dy / 2;
            }
            setCenterPos(newX, newY);
        }
    }

    void Node::contextMenuEvent(QGraphicsSceneContextMenuEvent * event) {
        QMenu menu;

        QAction *lockAction = menu.addAction(_pinned ? "Unlock" : "Lock");
        QAction *renameAction = menu.addAction("Rename...");
        QAction *selectedAction = menu.exec(event->screenPos());

        if(selectedAction == lockAction) {
            _pinned = !_pinned;
        } else if(selectedAction == renameAction) {
            bool ok;
            QString name = QInputDialog::getText(0,
                                                 "Set node name",
                                                 "Friendly node name :",
                                                 QLineEdit::Normal,
                                                 _friendlyName,
                                                 &ok);

            if(ok) {
                setName(name);
            }
        }
    }

    void Node::mousePressEvent(QGraphicsSceneMouseEvent * event) {
        QGraphicsItemGroup::mousePressEvent(event);
        if(event->button() == Qt::LeftButton) {
            _timeElapsedMouseMove.invalidate();
            _isBeingMoved = true;
        } else if(event->button() == Qt::MiddleButton) {
            _pinned = !_pinned;
        }
    }

    void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent * event) {
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

    void Node::mouseMoveEvent(QGraphicsSceneMouseEvent * event) {
        if(_isBeingMoved) {
            Link *link;

            if(_timeElapsedMouseMove.isValid() == false)
                _timeElapsedMouseMove.start();
            else {
                qint64 elapsedTime = _timeElapsedMouseMove.restart();

                if(elapsedTime && !_pinned) {
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

    QVariant Node::itemChange(GraphicsItemChange change, const QVariant & value) {
        if(change == ItemPositionChange && scene()) {
            // value is the new position.
            QPointF newPos = value.toPointF();
            QRectF rect = scene()->sceneRect();

            if(!rect.contains(newPos)) {
                // Keep the item inside the scene rect.
                // TODO: hardcoded 32 as Max Elipse Size
                newPos.setX(qMin(rect.right() - 32, qMax(newPos.x(), rect.left())));
                newPos.setY(qMin(rect.bottom() - 32, qMax(newPos.y(), rect.top())));
                return newPos;
            }
        }
        return QGraphicsItem::itemChange(change, value);
    }

    void Node::setNodeData(di_node_t * data, int version) {
        _nodeData = data;
        _version = version;
    }

}
