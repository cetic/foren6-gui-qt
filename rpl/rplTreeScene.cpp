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

#include "rplTreeScene.h"
#include "rplNode.h"
#include <rpl_packet_parser.h>
#include <QGraphicsTextItem>
#include <uthash.h>
#include <utlist.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "rplLink.h"
#include <math.h>

#include <QApplication>
#include <QVector2D>
#include <QPainter>
#include <QFileInfo>

namespace rpl {

    TreeScene::TreeScene(NetworkInfoManager * networkInfoManager) : _networkInfoManager(networkInfoManager) {
        _updateDagsTimer.setInterval(40);
        _updateDagsTimer.setSingleShot(false);
        connect(&_updateDagsTimer, SIGNAL(timeout()), this, SLOT(updateNodePositions()));
        _updateDagsTimer.start();
        layout = 0;
        background = new QPixmap();
        scale = 1.0;
        showNodeInfo = false;
    } void TreeScene::setBackground(QString newBackgroundFile) {
        backgroundFile = newBackgroundFile;
        delete background;

        background = new QPixmap(backgroundFile);
        if(background->rect().isValid()) {
            setSceneRect(background->rect());
        }
        emit backgroundChangeEvent();
    }

    void TreeScene::setLayout(QString layoutFile, QSettings * newLayout) {
        layout = newLayout;

        backgroundFile = layout->value("background", "").toString();
        if ( ! QDir::isAbsolutePath(backgroundFile) ) {
            QDir layoutPath = QFileInfo(layoutFile).dir();
            backgroundFile = layoutPath.filePath(backgroundFile);
        }
        setBackground(backgroundFile);
        scale = layout->value("scale", 1.0).toFloat();
        Node *node;

        //Clean up stage
        QList <Node * > _nodesToRemove;
        foreach(node, _nodes) {
            if (!node->seen()) {
                _nodesToRemove.append(node);
            }
        }
        foreach(node, _nodesToRemove) {
            removeNode(node);
            delete node;
        }
        QStringList nodes = layout->childGroups();
        foreach(QString node_id, nodes) {
            bool ok;
            addr_wpan_t node_addr = node_id.toUpper().toULongLong(&ok, 16);
            if (ok) {
                layout->beginGroup(node_id);
                node = getNode( node_addr );
                if (!node) {
                    node = new Node(_networkInfoManager, node_addr);
                    doAddNode(node);
                }
                if(layout->contains("x")) {
                    node->setPos(layout->value("x", 0).toFloat() * scale, layout->value("y", 0).toFloat() * scale);
                }
                node->setLocked(layout->value("locked", true).toBool());
                node->setName(layout->value("name", "").toString());
                node->setKnown(true);
                layout->endGroup();
            } else {
                printf("invalid node_id: %s\n", qPrintable(node_id));
            }
        }
    }

    void TreeScene::getLayout(QSettings * newLayout) {
        layout = newLayout;
        layout->setValue("background", backgroundFile);
        layout->setValue("scale", scale);
        Node *node;

        foreach(node, _nodes) {
            layout->beginGroup(QString::number(node->wpan_address(), 16));
            layout->setValue("x", node->x() / scale);
            layout->setValue("y", node->y() / scale);
            layout->setValue("locked", node->isLocked());
            layout->setValue("name", node->getName());
            layout->endGroup();
        }
    }

    void TreeScene::clearLayout() {
        setBackground(QString());

        foreach(Node *node, _nodes) {
            node->setLocked(false);
            node->setName("");
            node->setKnown(false);
        }
        layout = 0;
        scale = 1.0;
    }

    void TreeScene::drawBackground(QPainter * painter, const QRectF & rect) {
        painter->save();
        painter->drawPixmap(rect, *background, rect);
        painter->restore();
    }

    void TreeScene::setNodeMovement(bool moving) {
        if(!moving && _updateDagsTimer.isActive()) {
            _updateDagsTimer.stop();
        }
        if(moving && !_updateDagsTimer.isActive()) {
            _updateDagsTimer.start();
        }
    }

    void TreeScene::setNodeInfo(bool showInfo) {
        showNodeInfo = showInfo;
        Node *node;

        foreach(node, _nodes) {
            node->showInfoText(showInfo);
        }
    }

    void TreeScene::doAddNode(Node * node) {
        //qDebug("Adding Node %p %llX", node, node_get_mac64(node->getNodeData()));
        addItem(node);
        node->showInfoText(showNodeInfo);
        _nodes.insert(node->wpan_address(), node);
    }

    void TreeScene::addNode(Node * node) {
        doAddNode(node);
        if(layout) {
            layout->beginGroup(QString::number(node->wpan_address(), 16));
            if(layout->contains("x")) {
                node->setPos(layout->value("x", 0).toFloat() * scale, layout->value("y", 0).toFloat() * scale);
            }
            node->setLocked(layout->value("locked", false).toBool());
            node->setName(layout->value("name", "").toString());
            layout->endGroup();
        }
    }

    void TreeScene::addLink(Link * link) {
        //qDebug("Adding Link %p %llX -> %llX", link, link->getLinkData()->key.ref.child.wpan_address, link->getLinkData()->key.ref.parent.wpan_address);
        addItem(link);

        QPair < addr_wpan_t, addr_wpan_t > linkKey;
        linkKey.first = link_get_key(link->getLinkData())->ref.child.wpan_address;
        linkKey.second = link_get_key(link->getLinkData())->ref.parent.wpan_address;
        _links.insert(linkKey, link);
    }

    void TreeScene::removeNode(Node * node) {
        //qDebug("Removing Node %p", node);
        removeItem(node);
        _nodes.remove(node->wpan_address());
    }

    void TreeScene::removeAllNodes(bool keepKnownNodes) {
        if ( keepKnownNodes ) {
            QList <Node * > _nodesToRemove;
            foreach(Node *node, _nodes) {
                if (!node->known()) {
                    _nodesToRemove.append(node);
                } else {
                    QPen pen(QColor(128, 128, 128));
                    QBrush brush;
                    QFont font = QApplication::font();
                    font.setPointSize(8);
                    node->setSeen(false);
                    node->setNodeData(0, 0);
                    node->setPen(pen);
                    node->setBrush(brush);
                    node->setFont(font);
                    node->setTextColor(QColor(128, 128, 128));
                }
            }
            foreach(Node *node, _nodesToRemove) {
                removeNode(node);
                delete node;
            }
        } else {
            foreach(Node *node, _nodes) {
                removeItem(node);
                delete node;
            }
            _nodes.clear();
        }
    }

    void TreeScene::removeLink(Link * link) {
        //qDebug("Removing Link %p, raw link %p", link, link->getLinkData());
        removeItem(link);

        QPair < addr_wpan_t, addr_wpan_t > linkKey;
        linkKey.first = link_get_key(link->getLinkData())->ref.child.wpan_address;
        linkKey.second = link_get_key(link->getLinkData())->ref.parent.wpan_address;
        _links.remove(linkKey);
    }

    void TreeScene::removeAllLinks() {
        Link *link;

        foreach(link, _links) {
            removeItem(link);
            delete link;
        }
        _links.clear();
    }

    void TreeScene::clear(bool keepKnownNodes) {
        removeAllLinks();
        removeAllNodes(keepKnownNodes);
    }

    Node *TreeScene::getNode(addr_wpan_t address) {
        return _nodes.value(address, 0);
    }

    Link *TreeScene::getLink(addr_wpan_t child, addr_wpan_t parent) {
        QPair < addr_wpan_t, addr_wpan_t > linkKey(child, parent);

        return _links.value(linkKey, 0);
    }

    void TreeScene::updateNodePositions() {
        Link *currentLink;
        Node *n1, *n2;

        foreach(n1, _nodes) {
            foreach(currentLink, n1->links()) {
                QPointF pos1 = currentLink->from()->centerPos();
                QPointF pos2 = currentLink->to()->centerPos();
                qreal vx = pos2.x() - pos1.x();
                qreal vy = pos2.y() - pos1.y();
                qreal dist = sqrt(vx * vx + vy * vy);

                if(qAbs(dist) < 0.01)
                    dist = 0.01;
                //qreal factor = (100 - dist)/(dist * 3);
                qreal link_weight;

                /*if(currentLink->weight()/10 < 30)
                   link_weight = 30;
                   else */
                link_weight = currentLink->weight() / 10;
                if(link_weight != 0) {
                    qreal factor = (link_weight - dist) / (dist * 3);

                    currentLink->from()->incSpeed(-factor * vx, -factor * vy);
                    currentLink->to()->incSpeed(factor * vx, factor * vy);
                }
            }


            qreal dx = 0, dy = 0;

            foreach(n2, _nodes) {
                if(n1 == n2)
                    continue;
                QPointF pos1 = n1->centerPos();
                QPointF pos2 = n2->centerPos();
                qreal vx = pos1.x() - pos2.x();
                qreal vy = pos1.y() - pos2.y();
                qreal dist = vx * vx + vy * vy;

                if(dist == 0) {
                    dx += qrand() * 1.0 / RAND_MAX;
                    dy += qrand() * 1.0 / RAND_MAX;
                } else if(dist < 100 * 100) {
                    dx += vx / dist;
                    dy += vy / dist;
                }
            }

            qreal dist = dx * dx + dy * dy;

            if(dist > 0) {
                dist = sqrt(dist) / 2;
                n1->incSpeed(10 * dx / dist, 10 * dy / dist);
            }
            //Gravity
            //n1->incSpeed(0, 1);

            n1->updatePosition();
        }
    }

    bool TreeScene::hasValidBackground() {
        return this->background->rect().isValid();
    }

}
