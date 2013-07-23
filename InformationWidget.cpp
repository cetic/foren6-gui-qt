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
	connect(ui->messageTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onMessageLogDoubleClicked(QModelIndex)));
	connect(ui->filterEdit, SIGNAL(textChanged(QString)), this, SLOT(onFilterTextChanged(QString)));
}

InformationWidget::~InformationWidget()
{
	delete ui;
}

void InformationWidget::addMessage(int version, const QString& type, const QString& message) {
	messageLog->addMessage(version, type, message);
}

void InformationWidget::onMessageLogDoubleClicked(QModelIndex index) {
	emit setCurrentVersion(messageLog->getVersion(index.row()));
}

void InformationWidget::onFilterTextChanged(QString newText) {
	messageLog->setFilter(newText);
}
