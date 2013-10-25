#ifndef INFORMATIONWIDGET_H
#define INFORMATIONWIDGET_H

#include <QDockWidget>
#include <QModelIndex>
#include <stdint.h>
#include "EventLog.h"

namespace Ui {
class InformationWidget;
}

class InformationWidget : public QDockWidget
{
	Q_OBJECT
	
public:
	explicit InformationWidget(QWidget *parent = 0);
	~InformationWidget();


public slots:
	void addMessage(rpl::Event *newMsg);
	void clearMessages();
	void toggleAutoScroll();

signals:
	void setCurrentVersion(int version);
	void messageSelected(rpl::Event *  event);

protected slots:
    void onMessageLogClicked(QModelIndex index);
	void onMessageLogDoubleClicked(QModelIndex index);
	void onFilterTextChanged(QString newText);
	void rowsInserted();
	
private:
	Ui::InformationWidget *ui;
	EventLog *messageLog;
	bool autoScroll;
};

#endif // INFORMATIONWIDGET_H
