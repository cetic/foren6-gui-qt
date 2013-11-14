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
 *         Opened sources handler
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#ifndef OPENEDSNIFFERSMODEL_H
#define OPENEDSNIFFERSMODEL_H

#include <QAbstractTableModel>
#include <QUrl>
#include <QList>

#include "rpl_packet_parser.h"

class OpenedSniffersModel:public QAbstractTableModel {
  public:
    struct SnifferInterface {
        interface_t *interface;
        ifreader_t sniffer_handle;
        QUrl url;
    };

  public:
      OpenedSniffersModel(QObject * parent = 0);

    void append(QUrl url, interface_t * interface, ifreader_t sniffer_handle);
    interface_t *getInterface(int index);
    ifreader_t getHandle(int index);
    void remove(int index);
      QList < SnifferInterface > getOpenedSniffers() {
        return openedSniffers;
    } int rowCount(const QModelIndex & parent = QModelIndex())const;
    int columnCount(const QModelIndex & parent = QModelIndex())const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  private:
    QList < SnifferInterface > openedSniffers;


};

#endif // OPENEDSNIFFERSMODEL_H
