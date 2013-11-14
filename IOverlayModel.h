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
 *         Visual Effects for Overlays
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#ifndef IOVERLAYMODEL_H
#define IOVERLAYMODEL_H

#include <QPen>
#include <QBrush>
#include <QFont>
#include <QColor>
#include <QGraphicsTextItem>

namespace rpl {
    class Node;
    class Link;
} class IOverlayModel {
    //Methods return true if pen, font and/or color contain relevant data or false to keep the current object's style
  public:
    virtual ~ IOverlayModel() {
    } virtual int neededPasses() {
        return 1;
    }

    virtual void initPasses() {
    };

    virtual bool nodeCirclePen(rpl::Node * node, QPen * newPen, QBrush * newBrush);
    virtual bool nodeTextPen(rpl::Node * node, QFont * newFont, QColor * newColor);
    virtual bool linkPen(rpl::Link * link, QPen * newPen);
    virtual bool nodeInfoText(rpl::Node * node, QString * infoText);
};

#endif // IOVERLAYMODEL_H
