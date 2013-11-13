/*
 * Copyright (c) 2013, CETIC.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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
