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
 *         Traffic Overlay
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include "TrafficOverlay.h"
#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include <QApplication>

TrafficOverlay::TrafficOverlay()
{
    max_packets_encountered = 0;
}

bool
TrafficOverlay::nodeCirclePen(rpl::Node * node, QPen * newPen, QBrush * newBrush)
{
    if(!newPen || !newBrush)
        return false;

    int packet_count = node_get_sixlowpan_statistics(node->getNodeData())->packet_count;

    if(packet_count > max_packets_encountered) {
        max_packets_encountered = packet_count;
        //invalidate();
    }

    if(node->isSelected())
        *newPen = QPen(QColor(Qt::darkBlue));
    else {
        int color;

        if(max_packets_encountered != 0) {
            color = 240 - qMin(packet_count * 240 / max_packets_encountered, 240);
        } else {
            //force black instead of divide by 0 error
            color = 0;
        }
        *newPen = QPen(QColor(color, color, color));
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
TrafficOverlay::nodeInfoText(rpl::Node * node, QString * infoText)
{
    int packet_count = node_get_sixlowpan_statistics(node->getNodeData())->packet_count;

    if(infoText) {
        *infoText = QString::number(packet_count);
    }
    return true;
}
