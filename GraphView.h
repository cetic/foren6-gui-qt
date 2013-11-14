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
 *         Graph View Interface
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QWidget>
#include "rpl/rplNetworkInfoManager.h"


namespace Ui {
    class GraphView;
} class GraphView:public QWidget {
  Q_OBJECT public:
    explicit GraphView(QWidget * parent = 0);
    ~GraphView();
    void setNetworkManager(rpl::NetworkInfoManager * networkMgr);

    protected slots:void onLayoutComboChange(int index);
    void bindSceneToView();

  private:
      Ui::GraphView * ui;
      rpl::NetworkInfoManager * networkMgr;
    void resizeEvent(QResizeEvent * e);
};

#endif // GRAPHVIEW_H
