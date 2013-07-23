#ifndef INFORMATIONWIDGET_H
#define INFORMATIONWIDGET_H

#include <QDockWidget>
#include <QModelIndex>
#include "stdint.h"

namespace Ui {
class InformationWidget;
}

class EventLog;

class InformationWidget : public QDockWidget
{
	Q_OBJECT
	
public:
	explicit InformationWidget(QWidget *parent = 0);
	~InformationWidget();

public slots:
	void addMessage(int version, const QString& type, const QString& message);

signals:
	void setCurrentVersion(int version);

protected slots:
	void onMessageLogDoubleClicked(QModelIndex index);
	void onFilterTextChanged(QString newText);
	
private:
	Ui::InformationWidget *ui;
	EventLog *messageLog;
};

#endif // INFORMATIONWIDGET_H
