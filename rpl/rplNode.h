/*
 * This file is part of Foren6, a 6LoWPAN Diagnosis Tool
 * Copyright (C) 2013, CETIC
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
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

namespace rpl {

    class Link;
    class NetworkInfoManager;

    class Node:public QGraphicsItemGroup {
      public:
        Node(NetworkInfoManager * networkInfoManager, addr_wpan_t wpan_address);
         ~Node();

        void addLink(Link * link) {
            _links.append(link);
        } void removeLink(Link * link) {
            _links.removeOne(link);
        } void setCenterPos(QPointF newpos);
        void setCenterPos(qreal x, qreal y);

        QPointF centerPos() const;
        qreal centerX() const {
            return centerPos().x();
        } qreal centerY() const {
            return centerPos().y();
        } void setPos(qreal x, qreal y);

        void incSpeed(qreal x, qreal y);

        qreal speedX() {
            return _dx;
        }
        qreal speedY() {
            return _dy;
        }

        qreal radius() {
            return _ellipse.rect().width() / 2;
        }

        addr_wpan_t wpan_address() {
            return _wpan_address;
        }

        di_node_t *getNodeData() {
            return _nodeData;
        }
        void setNodeData(di_node_t * data, int version);

        QList < Link * >links() {
            return _links;
        }
        int getVersion() {
            return _version;
        }

        bool seen() const { return _seen; }
        void setSeen(bool seen) { _seen = seen; }

        bool known() const { return _known; }
        void setKnown(bool known) { _known = known; }

        void setPen(QPen pen) {
            _ellipse.setPen(pen);
        }
        void setBrush(QBrush brush) {
            _ellipse.setBrush(brush);
        }
        void setFont(QFont font) {
            _label.setFont(font);
            _infoLabel.setFont(font);
        }
        void setTextColor(QColor color);
        void setInfoText(QString infoText);
        void showInfoText(bool show);

        bool isSelected() {
            return _isSelected;
        }
        void setSelected(bool selected) {
            _isSelected = selected;
        }

        void setName(QString const &name);
        QString getName() const {
            return _friendlyName;
        } void setLocked(bool locked) {
            _pinned = locked;
        }
        bool isLocked() const {
            return _pinned;
        } public slots:void updatePosition();

      protected:
        void mousePressEvent(QGraphicsSceneMouseEvent * event);
        void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
        void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
        void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);
        void alignLabels();
        QVariant itemChange(GraphicsItemChange change, const QVariant & value);

      private:
        NetworkInfoManager * _networkInfoManager;
        addr_wpan_t _wpan_address;
        QElapsedTimer _timeElapsedMouseMove;    //Since last pos using mouse
        di_node_t *_nodeData;
        QGraphicsEllipseItem _ellipse;
        QGraphicsSimpleTextItem _label;
        QGraphicsSimpleTextItem _infoLabel;
        qreal _dx, _dy;
        bool _seen;
        bool _known;

        QList < Link * >_links;
        qreal _maxSize;

        QString _friendlyName;
        bool _isBeingMoved;
        bool _pinned;
        bool _isSelected;
        bool _showInfoText;

        //debug
        int _version;           //the version of the node data
        char guard[5];
    };

}

#endif // RPL_NODE_H
