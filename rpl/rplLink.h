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
 *         Parent-Child Link paint & physics engine
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#ifndef RPLLINK_H
#define RPLLINK_H

#include <data_info/link.h>
#include <QGraphicsLineItem>
#include <QPen>

namespace rpl
{

class Node;

class Link : public QGraphicsLineItem {
    public:
        Link(di_link_t *link, int version, Node *from, Node *to);
        virtual ~Link();

        qreal weight() { return link_get_deprecated(_link) ? 0 : 300; } //{ return link_get_metric(_link)->value; }
        Node *to() { return _to; }
        Node *from() { return _from; }
        di_link_t *getLinkData() { return _link; }
        void setLinkData(di_link_t *link, int version);
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

    public:
        //debug
        int _version;  //the version of the link data
        char guard[5];
    };

}

#endif // RPLLINK_H
