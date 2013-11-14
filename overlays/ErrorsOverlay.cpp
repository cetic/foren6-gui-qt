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
 *         Errors Overlay
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include "ErrorsOverlay.h"

#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include <QApplication>

ErrorsOverlay::ErrorsOverlay()
{
    maxErrorCount = 0;
}

bool
ErrorsOverlay::nodeCirclePen(rpl::Node * node, QPen * newPen, QBrush * newBrush)
{
    if(!newPen || !newBrush)
        return false;

    if(!node->getNodeData())
        return false;

    int errorCount = getErrorCount(node);

    if(maxErrorCount < errorCount)
        maxErrorCount = errorCount;

    if(node->isSelected())
        *newPen = QPen(QColor(Qt::darkBlue));
    else {
        int color;

        if(maxErrorCount > 0 && errorCount)
            color = qMin(errorCount * 192 / maxErrorCount, 192) + 63;
        else
            color = 0;
        *newPen = QPen(QColor(color, 0, 0));
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
ErrorsOverlay::nodeInfoText(rpl::Node * node, QString * infoText)
{
    int errorCount = getErrorCount(node);

    if(infoText) {
        *infoText = QString::number(errorCount);
    }
    return true;
}
