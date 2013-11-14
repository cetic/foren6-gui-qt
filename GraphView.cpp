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

#include "GraphView.h"
#include "ui_GraphView.h"

#include "overlays/DodagOverlay.h"
#include "overlays/NormalOverlay.h"
#include "overlays/RplInstanceOverlay.h"
#include "overlays/ParentChildrenOverlay.h"
#include "overlays/RankDepthOverlay.h"
#include "overlays/TrafficOverlay.h"
#include "overlays/MaxDaoIntervalOverlay.h"
#include "overlays/MaxDioIntervalOverlay.h"
#include "overlays/RankErrorOverlay.h"
#include "overlays/RouteLoopErrorOverlay.h"
#include "overlays/IPMismatchErrorOverlay.h"
#include "overlays/DodagMismatchErrorOverlay.h"

GraphView::GraphView(QWidget * parent):
QWidget(parent), ui(new Ui::GraphView)
{
    ui->setupUi(this);
    connect(ui->overlayCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onLayoutComboChange(int)));
}

GraphView::~GraphView()
{
    delete ui;
}

void
GraphView::setNetworkManager(rpl::NetworkInfoManager * networkMgr)
{
    ui->nodeGraphView->setScene(networkMgr->scene());
    this->networkMgr = networkMgr;
    bindSceneToView();
    connect(this->networkMgr->scene(), SIGNAL(backgroundChangeEvent()), this, SLOT(bindSceneToView()));
}

void
GraphView::onLayoutComboChange(int index)
{
    switch (index) {
    case 0:
        networkMgr->changeOverlay(new NormalOverlay);
        break;

    case 1:
        networkMgr->changeOverlay(new DodagOverlay);
        break;

    case 2:
        networkMgr->changeOverlay(new RplInstanceOverlay);
        break;

    case 3:
        networkMgr->changeOverlay(new RankDepthOverlay);
        break;

    case 4:
        networkMgr->changeOverlay(new TrafficOverlay);
        break;

    case 5:
        networkMgr->changeOverlay(new MaxDaoIntervalOverlay);
        break;

    case 6:
        networkMgr->changeOverlay(new MaxDioIntervalOverlay);
        break;

    case 7:
        networkMgr->changeOverlay(new RankErrorOverlay);
        break;

    case 8:
        networkMgr->changeOverlay(new RouteLoopErrorOverlay);
        break;

    case 9:
        networkMgr->changeOverlay(new IPMismatchErrorOverlay);
        break;

    case 10:
        networkMgr->changeOverlay(new DodagMismatchErrorOverlay);
        break;

    case 11:
        networkMgr->changeOverlay(new ParentChildrenOverlay);
        break;
    }
}

void
GraphView::resizeEvent(QResizeEvent * e)
{
    bindSceneToView();
}

void
GraphView::bindSceneToView()
{
    if(!this->networkMgr->scene()->hasValidBackground()) {
        QRect _rect = this->rect();

        _rect.setWidth(_rect.width() * 0.9);
        _rect.setHeight(_rect.height() * 0.9);
        _rect.translate(-_rect.width() / 2, -_rect.height() / 2);
        this->networkMgr->scene()->setSceneRect(_rect);
    }
}
