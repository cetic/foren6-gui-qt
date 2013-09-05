#ifndef OPENEDSNIFFERSMODEL_H
#define OPENEDSNIFFERSMODEL_H

#include <QAbstractTableModel>
#include <QUrl>
#include <QList>

#include "rpl_packet_parser.h"

class OpenedSniffersModel : public QAbstractTableModel
{
public:
	struct SnifferInterface {
		interface_t *interface;
		ifreader_t sniffer_handle;
		QUrl url;
	};

public:
	OpenedSniffersModel(QObject *parent = 0);

	void append(QUrl url, interface_t *interface, ifreader_t sniffer_handle);
	interface_t *getInterface(int index);
	ifreader_t getHandle(int index);
	void remove(int index);
	QList<SnifferInterface> getOpenedSniffers() { return openedSniffers; }

	int rowCount(const QModelIndex & parent = QModelIndex()) const;
	int columnCount(const QModelIndex & parent = QModelIndex()) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
	QList<SnifferInterface> openedSniffers;

	
};

#endif // OPENEDSNIFFERSMODEL_H
