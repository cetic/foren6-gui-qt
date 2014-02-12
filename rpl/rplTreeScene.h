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
#include <QDir>

class IOverlayModel;

namespace rpl {
    class Node;
    class Link;
    class NetworkInfoManager;

    class TreeScene:public QGraphicsScene {
      Q_OBJECT public:
        TreeScene(NetworkInfoManager * networkInfoManager);

        void addNode(Node * node);
        void addLink(Link * link);
        void removeNode(Node * node);
        void removeLink(Link * link);
        void removeAllLinks();
        void removeAllNodes(bool keepKnownNodes);
        void clear(bool keepKnownNodes);
        void setNodeMovement(bool moving);
        void setNodeInfo(bool showInfo);

        void setBackground(QString newBackgroundFile);
        bool hasValidBackground();
        void setLayout(QString layoutFile, QSettings * layout);
        void getLayout(QSettings * layout);
        void clearLayout();

        Node *getNode(addr_wpan_t address);
        Link *getLink(addr_wpan_t child, addr_wpan_t parent);

        const QHash < addr_wpan_t, Node * >&getNodes() {
            return _nodes;
        }
        const QHash < QPair < addr_wpan_t, addr_wpan_t >, Link * >&getLinks() {
            return _links;
        }
      protected:
        void drawBackground(QPainter * painter, const QRectF & rect);

      protected slots:
        void updateNodePositions();
        void doAddNode(Node * node);

      private:
        NetworkInfoManager * _networkInfoManager;
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
