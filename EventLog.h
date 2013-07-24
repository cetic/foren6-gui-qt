#ifndef EVENTLOG_H
#define EVENTLOG_H

#include <QString>
#include <QList>
#include <QAbstractTableModel>

class EventLog : public QAbstractTableModel
{
	Q_OBJECT
public:
	EventLog(QObject * parent = 0);

	void addMessage(int version, const QString& type, const QString& message);
	void setFilter(const QString& filter);

	int getVersion(int row);

	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
	struct Message {
		int version;
		QString type;
		QString message;
	};

	QList<Message*> messages;
	QList<Message*> filteredMessages;
	QString currentFilter;
};

#endif // EVENTLOG_H
