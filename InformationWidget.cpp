/*
 * This file is part of Foren6, a 6LoWPAN Diagnosis Tool
 * Copyright (C) 2013, CETIC
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * \file
 *         Dockable Information Widget
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include "InformationWidget.h"
#include "ui_InformationWidget.h"
#include "EventLog.h"
#include <stdint.h>

InformationWidget::InformationWidget(QWidget * parent):
QDockWidget(parent), ui(new Ui::InformationWidget), autoScroll(true), dialogsLinked(true)
{
    ui->setupUi(this);
    messageLog = new EventLog(this);
    rowChangedTimer = new QTimer(this);
    rowChangedTimer->setSingleShot(true);
    rowChangedTimer->setInterval(500);
    ui->messageTable->setModel(messageLog);
    connect(ui->messageTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onMessageLogDoubleClicked(QModelIndex)));
    connect(ui->filterEdit, SIGNAL(textChanged(QString)), this, SLOT(onFilterTextChanged(QString)));
    connect(ui->autoscrollButton, SIGNAL(toggled(bool)), this, SLOT(setAutoScroll(bool)));
    connect(ui->messageTable->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), this, SLOT(onSelectedRowChange(QModelIndex, QModelIndex)));
    connect(rowChangedTimer, SIGNAL(timeout()), this, SLOT(rowSelected()));
    setAttribute(Qt::WA_DeleteOnClose);
    lockManualSelection = false;
}

InformationWidget::~InformationWidget()
{
    delete ui;
}

void
InformationWidget::addMessage(rpl::Event * newMsg)
{
    messageLog->addMessage(newMsg);
    if(autoScroll) {
        QTimer::singleShot(1, this, SLOT(rowsInserted()));
    }
}

void
InformationWidget::rowsInserted()
{
    ui->messageTable->scrollTo(messageLog->index(messageLog->rowCount() - 1, 0));
}

void
InformationWidget::clearMessages()
{
    messageLog->clear();
}

void
InformationWidget::onMessageLogDoubleClicked(QModelIndex index)
{
    emit setCurrentVersion(messageLog->getVersion(index.row()));
}

void
InformationWidget::onFilterTextChanged(QString newText)
{
    messageLog->setFilter(newText);
}

void
InformationWidget::setAutoScroll(bool val)
{
    autoScroll = val;
}

void
InformationWidget::onChangeCurrentVersion(int version)
{
    int start, stop;

    //a RPL version can span multiple rows in the information widget
    findVersionIndexes(version, start, stop);
    //override current selection only if currently selected row is not within the new version's rows
    if(currentRow < start || currentRow > stop) {
        lockManualSelection = true;
        ui->messageTable->selectRow(start);
        ui->messageTable->scrollTo(messageLog->index(start, 0));
        if(dialogsLinked && currentRow != -1) {
            _tempRow = start;
            rowChangedTimer->start();
        }
        lockManualSelection = false;
    }
}

void
InformationWidget::findVersionIndexes(int version, int &startIndex, int &stopIndex)
{
    startIndex = -1;
    stopIndex = -1;
    bool found = false;
    int rows = ui->messageTable->model()->rowCount();

    for(int i = 0; i < rows; ++i) {
        QModelIndex index = ui->messageTable->model()->index(i, 1);

        if(ui->messageTable->model()->data(index) == QString::number(version)) {
            if(!found) {
                startIndex = i;
                stopIndex = i;
                found = true;
            } else {
                stopIndex = i;
            }
        }
        if(found && i > stopIndex) {
            break;
        }
    }
}

void
InformationWidget::onSelectedRowChange(QModelIndex current, QModelIndex previous)
{
    currentRow = current.row();
    previousRow = previous.row();
    _tempRow = currentRow;
    ui->messageTable->scrollTo(messageLog->index(currentRow, 0));
    ui->messageTable->resizeColumnsToContents();
    rowChangedTimer->start();
}

void
InformationWidget::onToggleLinkDialogs(bool state)
{
    dialogsLinked = state;
}

void
InformationWidget::rowSelected()
{
    if(!lockManualSelection) {
        emit messageSelected(messageLog->at(messageLog->index(_tempRow, 0)));
    }
}
