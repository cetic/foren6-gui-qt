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

#include "IOverlayModel.h"

#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include <QApplication>

bool
IOverlayModel::nodeCirclePen(rpl::Node * node, QPen * newPen, QBrush * newBrush)
{
    if(!newPen || !newBrush)
        return false;

    if(!node->getNodeData())
        return false;

    if(node->isSelected())
        *newPen = QPen(QColor(Qt::darkBlue));
    else {
        *newPen = QPen(QColor(Qt::black));
    }

    if(node_get_rank(node->getNodeData()) == 256) {
        newPen->setWidth(3);
    } else {
        newPen->setWidth(2);
    }

    newBrush->setStyle(Qt::SolidPattern);
    newBrush->setColor(Qt::white);

    return true;
}

bool
IOverlayModel::nodeTextPen(rpl::Node * node, QFont * newFont, QColor * newColor)
{
    Q_UNUSED(node);
    Q_UNUSED(newColor);

    if(newFont) {
        *newFont = QApplication::font();
        newFont->setPointSize(8);
    }

    return true;
}

bool
IOverlayModel::linkPen(rpl::Link * link, QPen * newPen)
{
    if(link_get_deprecated(link->getLinkData())) {
        *newPen = QPen(Qt::lightGray, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    } else {
        *newPen = QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    }
    return true;
}

bool
IOverlayModel::nodeInfoText(rpl::Node * node, QString * infoText)
{
    Q_UNUSED(node);
    Q_UNUSED(infoText);
    return false;
}
