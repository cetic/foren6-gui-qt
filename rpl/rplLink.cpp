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
 *         Parent-Child Link paint & physics engine
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include "rplLink.h"
#include "rplNode.h"
#include <QPainter>
#include <math.h>
#include <QGraphicsScene>

namespace rpl {

    static const qreal Pi = 3.14159;


      Link::Link(di_link_t * link, int version, Node * from, Node * to):_link(link), _version(version) {
        link_set_user_data(link, this);

        _from = from;
        _to = to;
        Q_ASSERT(_from);
        Q_ASSERT(_to);

        _from->addLink(this);
        _to->addLink(this);

        setZValue(0);

        updatePosition();

        qstrcpy(guard, "link");
    } Link::~Link() {
        _to->removeLink(this);
        _from->removeLink(this);
        qstrcpy(guard, "knil");
        setLinkData(0, -20);
    }

    void Link::updatePosition() {
        QLineF line(_to->centerPos(), _from->centerPos());

        setLine(line);
    }

    QRectF Link::boundingRect() const {
        qreal extra = (pen().width() + 20) / 2.0;

          return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(), line().p2().y() - line().p1().y()))
        . normalized()
        . adjusted(-extra, -extra, extra, extra);
    } QPainterPath Link::shape() const {
        QPainterPath path = QGraphicsLineItem::shape();
          path.addPolygon(_arrowHead);
          return path;
    } void Link::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
        if((_from->centerPos() - _to->centerPos()).manhattanLength() < _to->radius())
            return;

        qreal arrowSize = 6;

        painter->setPen(pen());
        painter->setBrush(pen().color());


        QLineF currentLine(_from->centerPos(), _to->centerPos());
        qreal nodeRadius = _to->radius();

        QPointF intersectPoint((1 - nodeRadius / currentLine.length()) * currentLine.dx() + currentLine.p1().x(),
                               (1 - nodeRadius / currentLine.length()) * currentLine.dy() + currentLine.p1().y());

        QLineF centerLine(intersectPoint, _from->centerPos());


        double angle =::acos(centerLine.dx() / centerLine.length());

        if(centerLine.dy() >= 0)
            angle = (Pi * 2) - angle;

        QPointF arrowP1 = centerLine.p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
                                                    cos(angle + Pi / 3) * arrowSize);
        QPointF arrowP2 = centerLine.p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                                    cos(angle + Pi - Pi / 3) * arrowSize);

        _arrowHead.clear();
        _arrowHead << centerLine.p1() << arrowP1 << arrowP2;

        painter->drawLine(centerLine);
        painter->drawPolygon(_arrowHead);


        if(isSelected()) {
            painter->setPen(QPen(pen().color(), 1, Qt::DashLine));
            QLineF myLine = line();

            myLine.translate(0, 4.0);
            painter->drawLine(myLine);
            myLine.translate(0, -8.0);
            painter->drawLine(myLine);
        }
    }

    void Link::setLinkData(di_link_t * link, int version) {
        if(_link)
            link_set_user_data(_link, 0);
        _link = link;
        _version = version;
        if(link)
            link_set_user_data(link, this);
    }

}
