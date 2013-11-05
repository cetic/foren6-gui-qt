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
    void setAutoScroll(bool);
    void onChangeCurrentVersion(int);
    void onToggleLinkDialogs(bool);

signals:
	void setCurrentVersion(int version);
	void messageSelected(rpl::Event *  event);

protected slots:
    void onMessageLogClicked(QModelIndex index);
	void onMessageLogDoubleClicked(QModelIndex index);
	void onFilterTextChanged(QString newText);
	void rowsInserted();
    void onSelectedRowChange(QModelIndex,QModelIndex);
	
private:
	Ui::InformationWidget *ui;
	EventLog *messageLog;
	bool autoScroll;
    void findVersionIndexes(int, int&, int&);
    int currentRow;
    int previousRow;
    bool dialogsLinked;
};

#endif // INFORMATIONWIDGET_H
