#include "GraphView.h"
#include "ui_GraphView.h"

#include "overlays/DodagOverlay.h"
#include "overlays/NormalOverlay.h"
#include "overlays/RplInstanceOverlay.h"
#include "overlays/ParentChildrenOverlay.h"
#include "overlays/RankDepthOverlay.h"
#include "overlays/TrafficOverlay.h"
#include "overlays/MaxDaoIntervalOverlay.h"

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
			networkMgr->changeOverlay(new ParentChildrenOverlay);
			break;

		case 4:
			networkMgr->changeOverlay(new RankDepthOverlay);
			break;

		case 5:
			networkMgr->changeOverlay(new TrafficOverlay);
			break;

		case 6:
			networkMgr->changeOverlay(new MaxDaoIntervalOverlay);
			break;
	}
}
