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

GraphView::GraphView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphView)
{
	ui->setupUi(this);
	connect(ui->overlayCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onLayoutComboChange(int)));
}

GraphView::~GraphView()
{
	delete ui;
}

void GraphView::setNetworkManager(rpl::NetworkInfoManager *networkMgr) {
	ui->nodeGraphView->setScene(networkMgr->scene());
	this->networkMgr = networkMgr;
    bindSceneToView();
    connect(this->networkMgr->scene(), SIGNAL(backgroundChangeEvent()), this, SLOT(bindSceneToView()));
}

void GraphView::onLayoutComboChange(int index) {
	switch(index) {
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

void GraphView::resizeEvent(QResizeEvent * e) {
    bindSceneToView();
}

void GraphView::bindSceneToView() {
    if(!this->networkMgr->scene()->hasValidBackground()) {
        QRect _rect = this->rect();
        _rect.setWidth(_rect.width()*0.9);
        _rect.setHeight(_rect.height()*0.9);
        _rect.translate(-_rect.width()/2, -_rect.height()/2);
        this->networkMgr->scene()->setSceneRect(_rect);
    }
}
