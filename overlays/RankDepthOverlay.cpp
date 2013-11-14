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
 *         Rank Depth Overlay
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include "RankDepthOverlay.h"
#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include <QApplication>
#include <math.h>

bool
RankDepthOverlay::nodeCirclePen(rpl::Node * node, QPen * newPen, QBrush * newBrush)
{
    if(!newPen || !newBrush)
        return false;

    if(node->isSelected())
        *newPen = QPen(QColor(Qt::darkBlue));
    else {
        int rank = node_get_rank(node->getNodeData());
        int color = qMax(qMin((rank - 256) / 10, 255), 0);

        if(rank != 65535) {
            if(rank > 1756)
                color = qMin(rank * (255 - 150) / (10000 - 1756) + 150, 255);
            *newPen = QPen(QColor(color, color, color));
        } else {
            *newPen = QPen(QColor(128, 128, 128));
            newPen->setStyle(Qt::DotLine);
        }
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
RankDepthOverlay::nodeInfoText(rpl::Node * node, QString * infoText)
{
    if(infoText) {
        if(node_has_rank(node->getNodeData())) {
            int rank = node_get_rank(node->getNodeData());

            *infoText = QString::number(rank);
        } else {
            *infoText = "-";
        }
    }
    return true;
}
