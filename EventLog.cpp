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

#include "EventLog.h"

#include <pcap_reader.h>

EventLog::EventLog(QObject *parent) : QAbstractTableModel(parent)
{
}

void EventLog::addMessage(rpl::Event *newMsg) {
    messages.append(newMsg);

    if(currentFilter.isEmpty() || getEventString(0, newMsg, true).contains(currentFilter) || getEventString(1, newMsg, true).contains(currentFilter, Qt::CaseInsensitive) || getEventString(2, newMsg, true).contains(currentFilter, Qt::CaseInsensitive)) {
        beginInsertRows(QModelIndex(), filteredMessages.size()-1, filteredMessages.size()-1);
        filteredMessages.append(newMsg);
        endInsertRows();
    }
}

void EventLog::clear() {
    beginResetModel();
    filteredMessages.clear();
    messages.clear();
    endResetModel();
}

void EventLog::setFilter(const QString& filter) {
    rpl::Event *message;

    beginInsertRows(QModelIndex(), filteredMessages.size()-1, filteredMessages.size()-1);

    currentFilter = filter;
    filteredMessages.clear();

    foreach(message, messages) {
        if(getEventString(0, message, true).contains(filter, Qt::CaseInsensitive) ||
                getEventString(1, message, true).contains(filter, Qt::CaseInsensitive) ||
                getEventString(2, message, true).contains(filter, Qt::CaseInsensitive)) {
            filteredMessages.append(message);
        }
    }

    endResetModel();
}

int EventLog::getVersion(int row) {
    return filteredMessages.at(row)->version;
}

Qt::ItemFlags EventLog::flags(const QModelIndex& index) const {
    if(index.isValid())
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    else return 0;
}

int EventLog::columnCount(const QModelIndex& parent) const {
    if(parent.isValid())
        return 0;
    else return 3;
}

int EventLog::rowCount(const QModelIndex& parent) const {
    if(parent.isValid())
        return 0;
    else return filteredMessages.size();
}

rpl::Event* EventLog::at(const QModelIndex& index) const {
    if(!index.isValid() )
        return NULL;

    return filteredMessages.at(index.row());
}

QVariant EventLog::data(const QModelIndex& index, int role) const {
    rpl::Event *currentEvent;

    if(!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    currentEvent = filteredMessages.at(index.row());

    return getEventString(index.column(), currentEvent, false);
}

QVariant EventLog::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role != Qt::DisplayRole)
        return QVariant(QVariant::Invalid);

    if(orientation == Qt::Horizontal) {
        if(section == 0)
            return QString("Type");
        else if(section == 1)
            return QString("Version");
        else if(section == 2)
            return QString("Message");
        else return QVariant();
    } else return QVariant::fromValue(section);
}

QString EventLog::getPacketTypeString(rpl::Event * event) const {
    switch (event->packet_info.type) {
    case PT_ICMPv6_Unknown:
        return "ICMPv6";
    case PT_PING_ECHO:
        return "Ping request";
    case PT_PING_REPLY:
        return "Ping reply";
    case PT_NDP_RS:
        return "NDP RS";
    case PT_NDP_RA:
        return "NDP RA";
    case PT_NDP_NS:
        return "NDP NS";
    case PT_NDP_NA:
        return "NDP NA";
    case PT_NDP_Redirect:
        return "NDP Redirect";
    case PT_6ND_DAR:
        return "6ND DAR";
    case PT_6ND_DAC:
        return "6ND DAC";
    case PT_RPL_Unknown:
        return "RPL Unknown";
    case PT_DIS:
        return "RPL DIS";
    case PT_DIO:
        return "RPL DIO";
    case PT_DAO:
        return "RPL DAO";
    case PT_UDP:
        return "UDP";
    case PT_TCP:
        return "TCP";
    case PT_IPv6_Unknown:
    default:
        return "Unknown";
    }
}

QString EventLog::getEventString(int column, rpl::Event *event, bool for_search) const {
    QString eventType;

    switch(event->type) {
        case RET_Created:
            eventType = "created";
            break;

        case RET_Updated:
            eventType = "updated";
            break;

        case RET_Deleted:
            eventType = "deleted";
            break;
    }

    if(column == 0) {
        switch(event->object) {
            case rpl::Event::EO_Node: return QString("Node");
            case rpl::Event::EO_Dodag: return QString("Dodag");
            case rpl::Event::EO_RplInstance: return QString("RPL Instance");
            case rpl::Event::EO_Link: return QString("Link");
            case rpl::Event::EO_Packet: return QString("Packet");
            default: return QString("");
        }
    } else if(column == 1) {
        return QString::number(event->version);
    } else if(column == 2) {
        switch(event->object) {
            case rpl::Event::EO_Node:
                return QString("Node %1, wpan id = %2")
                        .arg(eventType)
                        .arg(node_get_mac64(event->as_node), 0, 16);

            case rpl::Event::EO_Dodag: {
                char buffer[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, &dodag_get_key(event->as_dodag)->ref.dodagid, buffer, INET6_ADDRSTRLEN);

                return QString("Dodag %1, dodagid = %2, version = %3")
                        .arg(eventType)
                        .arg(buffer)
                        .arg(dodag_get_key(event->as_dodag)->ref.version);
            }

            case rpl::Event::EO_RplInstance:
                return QString("RPL Instance %1, instance = %2")
                        .arg(eventType)
                        .arg(rpl_instance_get_key(event->as_instance)->ref.rpl_instance);

            case rpl::Event::EO_Link:
                return QString("Link %1 from %2 to %3")
                        .arg(eventType)
                        .arg(link_get_key(event->as_link)->ref.child.wpan_address, 0, 16)
                        .arg(link_get_key(event->as_link)->ref.parent.wpan_address, 0, 16);

            case rpl::Event::EO_Packet: {
                char *buffer;
                int data_size = 0;
                pcap_file_t pcap_handle;

                if(false && for_search == false) {
                    pcap_handle = pcap_parser_open("out.pcap");

                    if(pcap_handle == NULL)
                        return QString("Frame: %1").arg(event->packed_id+1);

                    pcap_parser_get(pcap_handle, event->packed_id, NULL, &data_size);
                    buffer = (char*)malloc(data_size);
                    pcap_parser_get(pcap_handle, event->packed_id, buffer, &data_size);
                    pcap_parser_close(pcap_handle);
                    pcap_handle = 0;
                    //packet_id + 1 to start from 1 as wireshark
                    return QString("Frame: %1, data=%2").arg(event->packed_id+1).arg(QString::fromLocal8Bit(QByteArray(buffer, data_size).toHex()));
                } else {
                    return QString("Type: %1 src:%2 dst: %3 frame: %4")
                            .arg(getPacketTypeString(event))
                            .arg(event->packet_info.src_wpan_address, 0, 16)
                            .arg(event->packet_info.dst_wpan_address, 0, 16)
                            .arg(event->packed_id+1);
                }

            }

            default:
                return QString("");
        }
    }

    return QString();
}
