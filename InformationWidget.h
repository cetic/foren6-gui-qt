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

#ifndef INFORMATIONWIDGET_H
#define INFORMATIONWIDGET_H

#include <QDockWidget>
#include <QModelIndex>
#include <stdint.h>
#include "EventLog.h"

namespace Ui {
    class InformationWidget;
} class InformationWidget:public QDockWidget {
  Q_OBJECT public:
    explicit InformationWidget(QWidget * parent = 0);
    ~InformationWidget();


    public slots:void addMessage(rpl::Event * newMsg);
    void clearMessages();
    void setAutoScroll(bool);
    void onChangeCurrentVersion(int);
    void onToggleLinkDialogs(bool);

      signals:void setCurrentVersion(int version);
    void messageSelected(rpl::Event * event);

    protected slots:void onMessageLogDoubleClicked(QModelIndex index);
    void onFilterTextChanged(QString newText);
    void rowsInserted();
    void onSelectedRowChange(QModelIndex, QModelIndex);
    void rowSelected();

  private:
      Ui::InformationWidget * ui;
    EventLog *messageLog;
    bool autoScroll;
    void findVersionIndexes(int, int &, int &);
    int currentRow;
    int previousRow;
    int _tempRow;
    bool dialogsLinked;
    QTimer *rowChangedTimer;
    bool lockManualSelection;
};

#endif // INFORMATIONWIDGET_H
