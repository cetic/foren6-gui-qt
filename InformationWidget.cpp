#include "InformationWidget.h"
#include "ui_InformationWidget.h"
#include "EventLog.h"
#include <stdint.h>

InformationWidget::InformationWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::InformationWidget)
{
	ui->setupUi(this);
	messageLog = new EventLog(this);
	ui->messageTable->setModel(messageLog);
	connect(ui->messageTable, SIGNAL(clicked(QModelIndex)), this, SLOT(onMessageLogClicked(QModelIndex)));
	connect(ui->messageTable, SIGNAL(clicked(QModelIndex)), this, SLOT(onMessageLogClicked(QModelIndex)));
	connect(ui->messageTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onMessageLogDoubleClicked(QModelIndex)));
	connect(ui->filterEdit, SIGNAL(textChanged(QString)), this, SLOT(onFilterTextChanged(QString)));
}

InformationWidget::~InformationWidget()
{
	delete ui;
}

void InformationWidget::addMessage(rpl::Event *newMsg) {
	messageLog->addMessage(newMsg);
}

void InformationWidget::clearMessages() {
	messageLog->clear();
}

void InformationWidget::onMessageLogClicked(QModelIndex index) {
    rpl::Event *  event = messageLog->at(index);
    emit messageSelected(event);
}

void InformationWidget::onMessageLogDoubleClicked(QModelIndex index) {
	emit setCurrentVersion(messageLog->getVersion(index.row()));
}

void InformationWidget::onFilterTextChanged(QString newText) {
	messageLog->setFilter(newText);
}
