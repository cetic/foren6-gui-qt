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

#ifndef PACKETWIDGET_H
#define PACKETWIDGET_H

#include <QDockWidget>
#include <QModelIndex>
#include <QProcess>
#include <QTreeView>
#include <stdint.h>
#include "rpl/rplNetworkInfoManager.h"

#include "SimpleTreeModel.h"

namespace Ui {
class PacketWidget;
}

class PacketWidget : public QDockWidget
{
	Q_OBJECT
	
public:
	explicit PacketWidget(QWidget *parent = 0);
	~PacketWidget();

public slots:
    void showPacket(rpl::Event *newMsg);

signals:

protected:
  void storeExpandState();
  void applyExpandState();
  void applyExpandState_sub(QStringList& expandedItems,
                          QTreeView* treeView,
                          QAbstractItemModel* model,
                          QModelIndex startIndex);

protected slots:
	
private:
	Ui::PacketWidget *ui;
	TreeModel *model;
	QStringList expandedItems;

};

#endif // PACKETWIDGET_H
