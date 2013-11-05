#include "InformationWidget.h"
#include "ui_InformationWidget.h"
#include "EventLog.h"
#include <stdint.h>

InformationWidget::InformationWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::InformationWidget),
    autoScroll(true),
    dialogsLinked(false)
{
	ui->setupUi(this);
	messageLog = new EventLog(this);
	ui->messageTable->setModel(messageLog);
	connect(ui->messageTable, SIGNAL(clicked(QModelIndex)), this, SLOT(onMessageLogClicked(QModelIndex)));
	connect(ui->messageTable, SIGNAL(clicked(QModelIndex)), this, SLOT(onMessageLogClicked(QModelIndex)));
	connect(ui->messageTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onMessageLogDoubleClicked(QModelIndex)));
	connect(ui->filterEdit, SIGNAL(textChanged(QString)), this, SLOT(onFilterTextChanged(QString)));
    connect(ui->autoscrollButton, SIGNAL(toggled(bool)), this, SLOT(setAutoScroll(bool)));
    connect(ui->messageTable->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(onSelectedRowChange(QModelIndex,QModelIndex)));
	setAttribute(Qt::WA_DeleteOnClose);
}

InformationWidget::~InformationWidget()
{
	delete ui;
}

void InformationWidget::addMessage(rpl::Event *newMsg) {
	messageLog->addMessage(newMsg);
    if ( autoScroll ) {
      QTimer::singleShot(1, this, SLOT(rowsInserted()));
    }
}

void InformationWidget::rowsInserted() {
  ui->messageTable->scrollTo(messageLog->index(messageLog->rowCount()-1, 0));
}

void InformationWidget::clearMessages() {
	messageLog->clear();
}

void InformationWidget::onMessageLogClicked(QModelIndex index) {
    rpl::Event *  event = messageLog->at(index);
    emit messageSelected(event);
    ui->messageTable->resizeColumnsToContents();
}

void InformationWidget::onMessageLogDoubleClicked(QModelIndex index) {
	emit setCurrentVersion(messageLog->getVersion(index.row()));
}

void InformationWidget::onFilterTextChanged(QString newText) {
	messageLog->setFilter(newText);
}

void InformationWidget::setAutoScroll(bool val) {
    autoScroll = val;
}

void InformationWidget::onChangeCurrentVersion(int version){
    int start, stop;
    //a RPL version can span multiple rows in the information widget
    findVersionIndexes(version, start, stop);
    //override current selection only if currently selected row is not within the new version's rows
    if (currentRow < start || currentRow > stop){
        ui->messageTable->selectRow(start);
        ui->messageTable->scrollTo(messageLog->index(start, 0));
        if (dialogsLinked && currentRow != -1){
            emit messageSelected(messageLog->at(messageLog->index(start,0)));
        }
    }
}

void InformationWidget::findVersionIndexes(int version, int &startIndex, int &stopIndex){
    startIndex = -1;
    stopIndex = -1;
    bool found = false;
    int rows = ui->messageTable->model()->rowCount();
    for(int i = 0; i < rows; ++i)
    {
        QModelIndex index = ui->messageTable->model()->index(i, 1);
        if(ui->messageTable->model()->data(index) == QString::number(version))
        {
            if (!found){
                startIndex = i;
                stopIndex = i;
                found = true;
            }
            else{
                stopIndex = i;
            }
        }
        if (found && i > stopIndex){
            break;
        }
    }
}

void InformationWidget::onSelectedRowChange(QModelIndex current, QModelIndex previous){
    currentRow = current.row();
    previousRow = previous.row();
}

void InformationWidget::onToggleLinkDialogs(bool state){
    dialogsLinked = state;
}
