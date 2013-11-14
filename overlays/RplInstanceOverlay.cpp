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
 *         RPL Instances Overlay
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include "RplInstanceOverlay.h"
#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include "rpl/rplNetworkInfoManager.h"
#include <QApplication>
#include <QCryptographicHash>

QColor
RplInstanceOverlay::rplInstanceToColor(int16_t instance)
{
    unsigned char color[3] = { 0 };

    QByteArray md5Hash = QCryptographicHash::hash(QByteArray((const char *)&instance, sizeof(int16_t)), QCryptographicHash::Md5);

    for(int i = 0; i < md5Hash.size(); i++) {
        color[i % 3] += md5Hash.at(i);
    }

    return QColor(color[0] / 2 + 64, color[1] / 2 + 64, color[2] / 2 + 64);
}

bool
RplInstanceOverlay::nodeCirclePen(rpl::Node * node, QPen * newPen, QBrush * newBrush)
{
    if(!newPen || !newBrush)
        return false;

    const di_dodag_ref_t *dodagRef = node_get_dodag(node->getNodeData());
    QColor dodagColor;

    if(dodagRef) {
        const di_dodag_t *dodag = rpl::NetworkInfoManager::getInstance()->getDodag(dodagRef);

        dodagColor = rplInstanceToColor(dodag_get_rpl_instance(dodag)->rpl_instance);
    } else
        dodagColor = QColor(Qt::black);

    if(node->isSelected())
        *newPen = QPen(QColor(Qt::darkBlue));
    else
        *newPen = QPen(dodagColor);

    if(node_get_rank(node->getNodeData()) == 256) {
        newPen->setWidth(3);
    } else {
        newPen->setWidth(2);
    }

    newBrush->setStyle(Qt::SolidPattern);
    newBrush->setColor(Qt::white);

    return true;
}
