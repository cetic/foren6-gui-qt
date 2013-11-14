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
} class PacketWidget:public QDockWidget {
  Q_OBJECT public:
    explicit PacketWidget(QWidget * parent = 0);
    ~PacketWidget();

    public slots:void showPacket(rpl::Event * newMsg);

  signals:protected:
    void storeExpandState();
    void applyExpandState();
    void applyExpandState_sub(QStringList & expandedItems, QTreeView * treeView, QAbstractItemModel * model, QModelIndex startIndex);

  protected slots:private:
      Ui::PacketWidget * ui;
    TreeModel *model;
    QStringList expandedItems;

};

#endif // PACKETWIDGET_H
