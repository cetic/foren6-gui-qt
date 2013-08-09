#include "EventLog.h"

EventLog::EventLog(QObject *parent) : QAbstractTableModel(parent)
{
}

void EventLog::addMessage(rpl::Event *newMsg) {
	messages.append(newMsg);

	if(currentFilter.isEmpty() || getEventString(0, newMsg).contains(currentFilter) || getEventString(1, newMsg).contains(currentFilter, Qt::CaseInsensitive)) {
		beginInsertRows(QModelIndex(), filteredMessages.size()-1, filteredMessages.size()-1);
		filteredMessages.append(newMsg);
		endInsertRows();
	}

}

void EventLog::setFilter(const QString& filter) {
	rpl::Event *message;

	beginInsertRows(QModelIndex(), filteredMessages.size()-1, filteredMessages.size()-1);

	currentFilter = filter;
	filteredMessages.clear();

	foreach(message, messages) {
		if(getEventString(0, message).contains(filter) || getEventString(1, message).contains(filter, Qt::CaseInsensitive)) {
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
	else return 2;
}

int EventLog::rowCount(const QModelIndex& parent) const {
	if(parent.isValid())
		return 0;
	else return filteredMessages.size();
}

QVariant EventLog::data(const QModelIndex& index, int role) const {
	rpl::Event *currentEvent;

	if(!index.isValid() || role != Qt::DisplayRole)
		return QVariant();

	currentEvent = filteredMessages.at(index.row());

	return getEventString(index.column(), currentEvent);
}

QVariant EventLog::headerData(int section, Qt::Orientation orientation, int role) const {
	if(role != Qt::DisplayRole)
		return QVariant(QVariant::Invalid);

	if(orientation == Qt::Horizontal) {
		if(section == 0)
			return QString("Type");
		else if(section == 1)
			return QString("Message");
		else return QVariant();
	} else return QVariant::fromValue(section);
}


QString EventLog::getEventString(int column, rpl::Event *event) const {
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

			case rpl::Event::EO_Packet:
				return QString("Packet");

			default:
				return QString("");
		}
	}

	return QString();
}
