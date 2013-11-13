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
 *         Scene containing the interactive network graph
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#pragma once

#include <QGraphicsScene>
#include <QModelIndex>
#include <QVariant>
#include <rpl_packet_parser.h>
#include <QTimer>
#include <QSettings>

class IOverlayModel;

namespace rpl {
    class Node;
    class Link;

    class TreeScene:public QGraphicsScene {
      Q_OBJECT public:
        TreeScene();

        void addNode(Node * node);
        void addLink(Link * link);
        void removeNode(Node * node);
        void removeLink(Link * link);
        void removeAllLinks();
        void removeAllNodes();
        void clear();

        void setBackground(QString newBackgroundFile);
        bool hasValidBackground();
        void setLayout(QSettings * layout);
        void getLayout(QSettings * layout);
        void clearLayout();

        Node *getNode(addr_wpan_t address);
        Link *getLink(addr_wpan_t child, addr_wpan_t parent);

        const QHash < addr_wpan_t, Node * >&getNodes() {
            return _nodes;
        } const QHash < QPair < addr_wpan_t, addr_wpan_t >, Link * >&getLinks() {
            return _links;
      } protected:
        void drawBackground(QPainter * painter, const QRectF & rect);

        protected slots:void updateNodePositions();
        void toggleNodeMovement();
        void toggleNodeInfo();

      private:
        QTimer _updateDagsTimer;
        QHash < addr_wpan_t, Node * >_nodes;
        QHash < QPair < addr_wpan_t, addr_wpan_t >, Link * >_links;
        QSettings *layout;
        QPixmap *background;
        QString backgroundFile;
        qreal scale;
        bool showNodeInfo;

      signals:
        void backgroundChangeEvent();
    };

}
