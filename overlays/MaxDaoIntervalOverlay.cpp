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
 *         Max DAO Interval Overlay
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include "MaxDaoIntervalOverlay.h"
#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include <QApplication>

MaxDaoIntervalOverlay::MaxDaoIntervalOverlay()
{
    max_dao_interval = 0;
}

bool
MaxDaoIntervalOverlay::nodeCirclePen(rpl::Node * node, QPen * newPen, QBrush * newBrush)
{
    if(!newPen || !newBrush)
        return false;

    double interval = node_get_rpl_statistics(node->getNodeData())->max_dao_interval;

    if(interval > max_dao_interval) {
        max_dao_interval = interval;
        //invalidate();
    }

    if(node->isSelected())
        *newPen = QPen(QColor(Qt::darkBlue));
    else if(max_dao_interval > 0) {
        int color = qMin(static_cast < int >(interval * 240 / max_dao_interval), 240);

        *newPen = QPen(QColor(color, color, color));
    } else {
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
MaxDaoIntervalOverlay::nodeInfoText(rpl::Node * node, QString * infoText)
{
    double interval = node_get_rpl_statistics(node->getNodeData())->max_dao_interval;

    if(infoText) {
        *infoText = QString::number(interval, 'f', 0);
    }
    return true;
}
