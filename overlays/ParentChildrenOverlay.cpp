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
 *         Parent-Child Overlay
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include "ParentChildrenOverlay.h"
#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include "rpl/rplNetworkInfoManager.h"
#include "data_info/hash_container.h"
#include "data_info/link.h"
#include <QApplication>

ParentChildrenOverlay::ParentChildrenOverlay()
{
}

bool
ParentChildrenOverlay::nodeCirclePen(rpl::Node * node, QPen * newPen, QBrush * newBrush)
{
    bool makeGray = true;

    if(node->isSelected())
        *newPen = QPen(QColor(Qt::darkBlue));
    else
        *newPen = QPen(QColor(Qt::red));

    if(node_get_rank(node->getNodeData()) == 256) {
        newPen->setWidth(3);
    } else {
        newPen->setWidth(2);
    }

    newBrush->setStyle(Qt::SolidPattern);
    newBrush->setColor(Qt::white);

    if(node->isSelected() == false) {
        rpl::Link * link;
        rpl::TreeScene * scene = dynamic_cast < rpl::TreeScene * >(node->scene());

        if(scene == 0) {
            qDebug("ERROR: SCENE IS NOT A rpl::TreeScene !");
            return false;
        }

        foreach(link, scene->getLinks()) {
            rpl::Node * from, *to;

            from = link->from();
            to = link->to();

            if((from == node && to->isSelected()) || (to == node && from->isSelected()))
                makeGray = false;
        }
    } else
        makeGray = false;

    if(makeGray) {
        newPen->setColor(Qt::gray);
    } else if(node->isSelected() == false) {
        newPen->setColor(Qt::black);
    }

    return true;
}

bool
ParentChildrenOverlay::linkPen(rpl::Link * link, QPen * newPen)
{
    Q_UNUSED(link);
    rpl::Node * to, *from;

    from = link->from();
    to = link->to();

    if((to && to->isSelected()) || (from && from->isSelected()))
        *newPen = QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    else
        *newPen = QPen(Qt::gray, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    return true;
}
