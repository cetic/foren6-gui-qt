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

PacketWidget::PacketWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::PacketWidget),
    model(0)
{
	ui->setupUi(this);
	ui->dataTree->setFont(QFont("Courier"));
    setAttribute(Qt::WA_DeleteOnClose);
}

PacketWidget::~PacketWidget()
{
	delete ui;
}

void PacketWidget::applyExpandState_sub(QStringList& expandedItems,
                          QTreeView* treeView,
                          QAbstractItemModel* model,
                          QModelIndex startIndex)
{
    foreach (QString item, expandedItems)
    {
        QModelIndexList matches = model->match( startIndex, Qt::DisplayRole, item );
        foreach (QModelIndex index, matches)
        {
            treeView->setExpanded( index, true );
            applyExpandState_sub(expandedItems,
                                 treeView,
                                 model,
                                 model->index( 0, 0, index ) );
        }
    }
}

void PacketWidget::applyExpandState() {
    ui->dataTree->setUpdatesEnabled(false);

    applyExpandState_sub( expandedItems,
                          ui->dataTree,
                          model,
                          model->index( 0, 0, QModelIndex() ) );

    ui->dataTree->setUpdatesEnabled(true);
}

void PacketWidget::storeExpandState() {
    expandedItems.clear();
    foreach (QModelIndex index, model->getPersistentIndexList())
    {
        if (ui->dataTree->isExpanded(index))
        {
             QString item = index.data(Qt::DisplayRole).toString();
             QString id;
             int colon = item.indexOf(':');
             if ( colon  != -1 ) {
                 id = item.left(colon);
             } else {
               int comma = item.indexOf(',');
               if ( comma != -1 ) {
                 id = item.left(comma);
               } else {
                 id = item;
               }
             }
             expandedItems << id;
        }
    }
}

void PacketWidget::showPacket(rpl::Event *event) {
    if (model) {
        storeExpandState();
    }
    TreeModel * oldModel = model;
    ui->dataTree->setModel(0);
    delete oldModel;
    model = 0;

    if ( event->object != rpl::Event::EO_Packet ) return;

    char ipv6string[INET6_ADDRSTRLEN];
    analyser_config_t config = *(rpl_tool_get_analyser_config());
    inet_ntop(AF_INET6, (const char*)&config.context0, ipv6string, INET6_ADDRSTRLEN);

    QString context = "6lowpan.context0:";
    context += ipv6string;
    QProcess tshark;
    tshark.start("tshark", QStringList() << "-V" << "-c" << "1" << "-r" << "out.pcap" << "-R" << (QString("frame.number==") + QString::number(event->packed_id+1)) << "-o" << context);
    if (!tshark.waitForStarted())
        return;
    if (!tshark.waitForFinished())
        return;

    QByteArray result = tshark.readAll();
    model = new TreeModel(result);
    ui->dataTree->setModel(model);
    applyExpandState();
    ui->dataTree->resizeColumnToContents(0);
}
