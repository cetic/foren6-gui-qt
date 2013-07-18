#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <rpl_packet_parser.h>
#include "rpl/rplNetworkInfoManager.h"
#include "RplDiagnosisTool.h"
#include <arpa/inet.h>

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
	static const di_node_t* lastNode = 0;
	QTreeWidgetItem *item;
	QStringList stringList;
	char ipv6string[INET6_ADDRSTRLEN];

//  When using version 0, the node has always the same address but change
//	if(node == lastNode)
//		return;
//	lastNode = node;

	ui->rplNodeInfoTree->clear();

	stringList << "Node";
	item = new QTreeWidgetItem(stringList);

	stringList.clear();
	stringList << "MAC Address" << QString::number(node_get_key(node)->ref.wpan_address, 16);
	item->addChild(new QTreeWidgetItem(stringList));

	stringList.clear();
	inet_ntop(AF_INET6, (const char*)node_get_local_ip(node), ipv6string, INET6_ADDRSTRLEN);
	stringList << "Link-Local IPv6 Address" << ipv6string;
	item->addChild(new QTreeWidgetItem(stringList));

	stringList.clear();
	inet_ntop(AF_INET6, (const char*)node_get_global_ip(node), ipv6string, INET6_ADDRSTRLEN);
	stringList << "Global IPv6 Address" << ipv6string;
	item->addChild(new QTreeWidgetItem(stringList));

	stringList.clear();
	stringList << "Grounded" << (node_get_grounded(node) ? "true" : "false");
	item->addChild(new QTreeWidgetItem(stringList));

	stringList.clear();
	stringList << QString("Metric (") + (node_get_metric(node)->type? node_get_metric(node)->type->name : "Unknown") + ")" << QString::number(node_get_metric(node)->value);
	item->addChild(new QTreeWidgetItem(stringList));

	stringList.clear();
	stringList << "Rank" << QString::number(node_get_rank(node));
	item->addChild(new QTreeWidgetItem(stringList));

	ui->rplNodeInfoTree->insertTopLevelItem(0, item);

	ui->rplNodeInfoTree->expandAll();
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
