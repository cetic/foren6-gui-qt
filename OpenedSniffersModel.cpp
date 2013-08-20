#include "OpenedSniffersModel.h"

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
			return openedSniffers.at(index.row()).url.queryItemValue("channel");
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