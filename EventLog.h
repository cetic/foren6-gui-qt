#ifndef EVENTLOG_H
#define EVENTLOG_H

#include <QString>
#include <QList>
#include <QAbstractTableModel>

#include "rpl/rplNetworkInfoManager.h"
#include <pcap_reader.h>

class EventLog : public QAbstractTableModel
{
	Q_OBJECT
public:
	EventLog(QObject * parent = 0);

	void addMessage(rpl::Event *newMsg);
	void setFilter(const QString& filter);

	int getVersion(int row);

	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

protected:
	QString getEventString(int column, rpl::Event *event) const;

private:
	QList<rpl::Event*> messages;
	QList<rpl::Event*> filteredMessages;
	QString currentFilter;
};

#endif // EVENTLOG_H
