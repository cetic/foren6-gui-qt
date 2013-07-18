#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <rpl_packet_parser.h>
#include "rpl/rplNetworkInfoManager.h"
#include "RplDiagnosisTool.h"

MainWindow::MainWindow(RplDiagnosisTool *rplDiagnosisTool) :
	QMainWindow(0),
	ui(new Ui::MainWindow),
	rplDiagnosisTool(rplDiagnosisTool)
{
	ui->setupUi(this);
	ui->infoSplitter->setStretchFactor(0, 0);
	ui->infoSplitter->setStretchFactor(1, 1);
	ui->logSplitter->setStretchFactor(0, 3);
	ui->logSplitter->setStretchFactor(1, 1);

	ui->rplTreeView->setScene(rplDiagnosisTool->getScene());
	connect(ui->actionStart, SIGNAL(triggered()), this, SLOT(onStartSniffer()));
	connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(onStopSniffer()));
	connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderMove(int)));
}

MainWindow::~MainWindow()
{
	rpl_tool_cleanup();
	delete ui;
}

void MainWindow::setNodeInfoTarget(const di_node_t* node) {

}

void MainWindow::onStartSniffer() {
	rplDiagnosisTool->onStartSniffer();
}

void MainWindow::onStopSniffer() {
	rplDiagnosisTool->onStopSniffer();
}

void MainWindow::onSliderMove(int value) {
	uint32_t version = value*rpldata_get_wsn_last_version()/ui->horizontalSlider->maximum();

	if(value == ui->horizontalSlider->maximum())
		version = 0;  //realtime mode
	else if(version == 0 && rpldata_get_wsn_last_version() > 1)
		version = 1; //When the slide is at left most, use the first version which is version 1 (version 0 is the realtime version)

	ui->logEdit->append(QString("Using version %1").arg(version));

	emit changeVersion(version);
}
