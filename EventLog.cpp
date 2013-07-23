#include "EventLog.h"

EventLog::EventLog(QObject *parent) : QAbstractTableModel(parent)
{
}

void EventLog::addMessage(int version, const QString& type, const QString& message) {
	Message *newMessage = new Message;
	newMessage->version = version;
	newMessage->type = type;
	newMessage->message = message;
	messages.append(newMessage);

	if(currentFilter.isEmpty() || type.contains(currentFilter) || message.contains(currentFilter, Qt::CaseInsensitive)) {
		beginInsertRows(QModelIndex(), filteredMessages.size()-1, filteredMessages.size()-1);
		filteredMessages.append(newMessage);
		endInsertRows();
	}

}

void EventLog::setFilter(const QString& filter) {
	Message *message;

	beginInsertRows(QModelIndex(), filteredMessages.size()-1, filteredMessages.size()-1);

	currentFilter = filter;
	filteredMessages.clear();

	foreach(message, messages) {
		if(message->type.contains(filter) || message->message.contains(filter, Qt::CaseInsensitive)) {
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
	QString value;

	if(!index.isValid() || role != Qt::DisplayRole)
		return QVariant();

	if(index.column() == 0)
		value = filteredMessages.at(index.row())->type;
	else if(index.column() == 1)
		value = filteredMessages.at(index.row())->message;

	return value;
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
