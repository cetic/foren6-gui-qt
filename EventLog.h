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
 *         Event Log
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#ifndef EVENTLOG_H
#define EVENTLOG_H

#include <QString>
#include <QList>
#include <QAbstractTableModel>

#include "rpl/rplNetworkInfoManager.h"
#include <pcap_reader.h>

class EventLog:public QAbstractTableModel {
  Q_OBJECT public:
    EventLog(QObject * parent = 0);

    void addMessage(rpl::Event * newMsg);
    void clear();
    void setFilter(const QString & filter);

    int getVersion(int row);

    virtual Qt::ItemFlags flags(const QModelIndex & index) const;

    virtual int columnCount(const QModelIndex & parent = QModelIndex())const;
    virtual int rowCount(const QModelIndex & parent = QModelIndex())const;
    virtual rpl::Event * at(const QModelIndex & index) const;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  protected:
      QString getEventString(int column, rpl::Event * event, bool for_search) const;
      QString getPacketTypeString(rpl::Event * event) const;

  private:
      QList < rpl::Event * >messages;
      QList < rpl::Event * >filteredMessages;
    QString currentFilter;
};

#endif // EVENTLOG_H
