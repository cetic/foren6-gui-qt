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
 *         DODAG Overlay
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include "DodagOverlay.h"
#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include <QApplication>
#include <QCryptographicHash>
#include <stdlib.h>

QColor
DodagOverlay::dodagToColor(const void *dodagid, int16_t version)
{
    unsigned char color[3] = { 0 };
    char rawDodagId[18];

    memcpy(rawDodagId, dodagid, 16);
    rawDodagId[16] = version & 0xFF;
    rawDodagId[17] = (version >> 8) & 0xFF;

    QByteArray dodagHash = QCryptographicHash::hash(QByteArray(rawDodagId, 18), QCryptographicHash::Sha1);

    for(int i = 0; i < dodagHash.size(); i++) {
        color[i % 3] = color[i % 3] + dodagHash.at(i) + color[i % 3] * dodagHash.at(i);
    }

    color[0] = qMax(-128, qMin(128, (int)((((int)color[0]) - 128) * 1.2))) + 128;
    color[1] = qMax(-128, qMin(128, (int)((((int)color[1]) - 128) * 1.2))) + 128;
    color[2] = qMax(-128, qMin(128, (int)((((int)color[2]) - 128) * 1.2))) + 128;

    return QColor(color[0], color[1], color[2]);
}

bool
DodagOverlay::nodeCirclePen(rpl::Node * node, QPen * newPen, QBrush * newBrush)
{
    if(!newPen || !newBrush)
        return false;

    const di_dodag_ref_t *dodagRef = node_get_dodag(node->getNodeData());
    QColor dodagColor;

    if(dodagRef)
        dodagColor = dodagToColor(&dodagRef->dodagid, dodagRef->version);
    else
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

bool
DodagOverlay::nodeInfoText(rpl::Node * node, QString * infoText)
{
    if(infoText) {
        const rpl_instance_config_t *config = node_get_instance_config(node->getNodeData());

        if(config && config->has_dio_config) {
            *infoText = QString::number(config->version_number);
        } else {
            *infoText = "-";
        }
    }
    return true;
}
