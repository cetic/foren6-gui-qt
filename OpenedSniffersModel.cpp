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

#include "OpenedSniffersModel.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QUrlQuery>
#endif

OpenedSniffersModel::OpenedSniffersModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

void OpenedSniffersModel::append(QUrl url, interface_t *interface, ifreader_t sniffer_handle) {
    SnifferInterface newInterface;

    newInterface.url = url;
    newInterface.interface = interface;
    newInterface.sniffer_handle = sniffer_handle;

    beginInsertRows(QModelIndex(), openedSniffers.size(), openedSniffers.size());
    openedSniffers.append(newInterface);
    endInsertRows();
}

interface_t *OpenedSniffersModel::getInterface(int index) {
    return openedSniffers.at(index).interface;
}

ifreader_t OpenedSniffersModel::getHandle(int index) {
    return openedSniffers.at(index).sniffer_handle;
}

void OpenedSniffersModel::remove(int index) {
    beginRemoveRows(QModelIndex(), index, index);
    openedSniffers.removeAt(index);
    endRemoveRows();
}


int OpenedSniffersModel::rowCount(const QModelIndex & parent) const {
    if(parent.isValid())
        return 0;
    else return openedSniffers.size();
}

int OpenedSniffersModel::columnCount(const QModelIndex & parent) const {
    if(parent.isValid())
        return 0;
    else return 4;
}

QVariant OpenedSniffersModel::data(const QModelIndex & index, int role ) const {
    if(index.parent().isValid() || role != Qt::DisplayRole)
        return QVariant();

    switch(index.column()) {
        case 0:
            return openedSniffers.at(index.row()).url.scheme();
        case 1:
            return openedSniffers.at(index.row()).url.path();
        case 2:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            return QUrlQuery(openedSniffers.at(index.row()).url).queryItemValue("channel");
#else
            return openedSniffers.at(index.row()).url.queryItemValue("channel");
#endif
        case 3:
            return openedSniffers.at(index.row()).url.toString();
        default:
            return QVariant();
    }
}

QVariant OpenedSniffersModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch(section) {
        case 0:
            return QString("Type");
        case 1:
            return QString("Path");
        case 2:
            return QString("Channel");
        case 3:
            return QString("URL");
        default:
            return QVariant();
    }
}
