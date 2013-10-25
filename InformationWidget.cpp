#include "InformationWidget.h"
#include "ui_InformationWidget.h"
#include "EventLog.h"
#include <stdint.h>

InformationWidget::InformationWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::InformationWidget),
    autoScroll(true)
{
	ui->setupUi(this);
	messageLog = new EventLog(this);
	ui->messageTable->setModel(messageLog);
	connect(ui->messageTable, SIGNAL(clicked(QModelIndex)), this, SLOT(onMessageLogClicked(QModelIndex)));
	connect(ui->messageTable, SIGNAL(clicked(QModelIndex)), this, SLOT(onMessageLogClicked(QModelIndex)));
	connect(ui->messageTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onMessageLogDoubleClicked(QModelIndex)));
	connect(ui->filterEdit, SIGNAL(textChanged(QString)), this, SLOT(onFilterTextChanged(QString)));
    connect(ui->autoscrollButton, SIGNAL(toggled(bool)), this, SLOT(setAutoScroll(bool)));
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
