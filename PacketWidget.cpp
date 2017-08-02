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
 *         Dockable Packet Dissector Widget
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include "PacketWidget.h"
#include "ui_PacketWidget.h"
#include <QProcess>
#include <stdint.h>
#include "SimpleTreeModel.h"

PacketWidget::PacketWidget(QWidget * parent):
QDockWidget(parent), ui(new Ui::PacketWidget), model(0)
{
    ui->setupUi(this);
    ui->dataTree->setFont(QFont("Courier"));
    setAttribute(Qt::WA_DeleteOnClose);
}

PacketWidget::~PacketWidget()
{
    delete ui;
}

void
PacketWidget::applyExpandState_sub(QStringList & expandedItems, QTreeView * treeView, QAbstractItemModel * model, QModelIndex startIndex)
{
    foreach(QString item, expandedItems) {
        QModelIndexList matches = model->match(startIndex, Qt::DisplayRole, item);

        foreach(QModelIndex index, matches) {
            treeView->setExpanded(index, true);
            applyExpandState_sub(expandedItems, treeView, model, model->index(0, 0, index));
        }
    }
}

void
PacketWidget::applyExpandState()
{
    ui->dataTree->setUpdatesEnabled(false);

    applyExpandState_sub(expandedItems, ui->dataTree, model, model->index(0, 0, QModelIndex()));

    ui->dataTree->setUpdatesEnabled(true);
}

void
PacketWidget::storeExpandState()
{
    expandedItems.clear();
    foreach(QModelIndex index, model->getPersistentIndexList()) {
        if(ui->dataTree->isExpanded(index)) {
            QString item = index.data(Qt::DisplayRole).toString();
            QString id;
            int colon = item.indexOf(':');

            if(colon != -1) {
                id = item.left(colon);
            } else {
                int comma = item.indexOf(',');

                if(comma != -1) {
                    id = item.left(comma);
                } else {
                    id = item;
                }
            }
            expandedItems << id;
        }
    }
}

void
PacketWidget::showPacket(rpl::Event * event)
{
    if(model) {
        storeExpandState();
    }
    TreeModel *oldModel = model;

    ui->dataTree->setModel(0);
    delete oldModel;

    model = 0;

    if(event->object != rpl::Event::EO_Packet)
        return;

    char ipv6string[INET6_ADDRSTRLEN];
    analyser_config_t config = *(rpl_tool_get_analyser_config());

    inet_ntop(AF_INET6, (const char *)&config.context0, ipv6string, INET6_ADDRSTRLEN);

    QString context = "6lowpan.context0:";

    context += ipv6string;
    QProcess tshark;

    QStringList parameters;
    if(rpl_tool_get_analyser_config()->old_tshark) {
        parameters = QStringList() << "-V" << "-c" << "1" << "-r" << "out.pcap" << "-R" << (QString("frame.number==") +
                                                                                            QString::number(event->packed_id + 1)) << "-o" << context;
    } else {
      parameters = QStringList() << "-V" << "-r" << "out.pcap" << "-Y" << (QString("frame.number==") +
                                                                           QString::number(event->packed_id + 1)) << "-o" << context;
    }

    tshark.start("tshark", parameters);

    if(!tshark.waitForStarted())
        return;
    if(!tshark.waitForFinished())
        return;

    QByteArray result = tshark.readAll();

    model = new TreeModel(result);
    ui->dataTree->setModel(model);
    applyExpandState();
    ui->dataTree->resizeColumnToContents(0);
}
