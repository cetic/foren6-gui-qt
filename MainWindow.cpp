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


	nodeInfoTree.rplInstanceMain = new QTreeWidgetItem(ui->rplNodeInfoTree);
	nodeInfoTree.rplInstanceMain->setText(0, "RPL Instance");
	nodeInfoTree.rplInstanceId = new QTreeWidgetItem(nodeInfoTree.rplInstanceMain);
	nodeInfoTree.rplInstanceId->setText(0, "Instance ID");
	nodeInfoTree.rplInstanceModeOfOperation = new QTreeWidgetItem(nodeInfoTree.rplInstanceMain);
	nodeInfoTree.rplInstanceModeOfOperation->setText(0, "Mode of Operation");

	nodeInfoTree.dodagMain = new QTreeWidgetItem(ui->rplNodeInfoTree);
	nodeInfoTree.dodagMain->setText(0, "DODAG");
	nodeInfoTree.dodagVersion = new QTreeWidgetItem(nodeInfoTree.dodagMain);
	nodeInfoTree.dodagVersion->setText(0, "Version");
	nodeInfoTree.dodagId = new QTreeWidgetItem(nodeInfoTree.dodagMain);
	nodeInfoTree.dodagId->setText(0, "DODAG ID");
	nodeInfoTree.dodagPrefix = new QTreeWidgetItem(nodeInfoTree.dodagMain);
	nodeInfoTree.dodagPrefix->setText(0, "Prefix");
	nodeInfoTree.dodagConfigAuthEnabled = new QTreeWidgetItem(nodeInfoTree.dodagMain);
	nodeInfoTree.dodagConfigAuthEnabled->setText(0, "Use Authentication");
	nodeInfoTree.dodagConfigPathControlSize = new QTreeWidgetItem(nodeInfoTree.dodagMain);
	nodeInfoTree.dodagConfigPathControlSize->setText(0, "Path Control Size");
	nodeInfoTree.dodagConfigDioIntervalMin = new QTreeWidgetItem(nodeInfoTree.dodagMain);
	nodeInfoTree.dodagConfigDioIntervalMin->setText(0, "DIO Interval Min");
	nodeInfoTree.dodagConfigDioIntervalMax = new QTreeWidgetItem(nodeInfoTree.dodagMain);
	nodeInfoTree.dodagConfigDioIntervalMax->setText(0, "DIO Interval Max");
	nodeInfoTree.dodagConfigDioRedundancyConstant = new QTreeWidgetItem(nodeInfoTree.dodagMain);
	nodeInfoTree.dodagConfigDioRedundancyConstant->setText(0, "DIO Redundancy Constant");
	nodeInfoTree.dodagConfigMaxRankIncrease = new QTreeWidgetItem(nodeInfoTree.dodagMain);
	nodeInfoTree.dodagConfigMaxRankIncrease->setText(0, "Max Rank Increase");
	nodeInfoTree.dodagConfigMinHopRankIncrease = new QTreeWidgetItem(nodeInfoTree.dodagMain);
	nodeInfoTree.dodagConfigMinHopRankIncrease->setText(0, "Min Hop Rank Increase");
	nodeInfoTree.dodagConfigDefaultLifetime = new QTreeWidgetItem(nodeInfoTree.dodagMain);
	nodeInfoTree.dodagConfigDefaultLifetime->setText(0, "Defaut Lifetime");
	nodeInfoTree.dodagConfigLifetimeUnit = new QTreeWidgetItem(nodeInfoTree.dodagMain);
	nodeInfoTree.dodagConfigLifetimeUnit->setText(0, "Lifetime Unit");
	nodeInfoTree.dodagConfigObjectiveFunction = new QTreeWidgetItem(nodeInfoTree.dodagMain);
	nodeInfoTree.dodagConfigObjectiveFunction->setText(0, "Objective Function");

	nodeInfoTree.nodeMain = new QTreeWidgetItem(ui->rplNodeInfoTree);
	nodeInfoTree.nodeMain->setText(0, "Node");
	nodeInfoTree.nodeMacAddress = new QTreeWidgetItem(nodeInfoTree.nodeMain);
	nodeInfoTree.nodeMacAddress->setText(0, "MAC Address");
	nodeInfoTree.nodeLocalIp = new QTreeWidgetItem(nodeInfoTree.nodeMain);
	nodeInfoTree.nodeLocalIp->setText(0, "Link-Local IP");
	nodeInfoTree.nodeGlobalIp = new QTreeWidgetItem(nodeInfoTree.nodeMain);
	nodeInfoTree.nodeGlobalIp->setText(0, "Global IP");
	nodeInfoTree.nodeMetric = new QTreeWidgetItem(nodeInfoTree.nodeMain);
	nodeInfoTree.nodeMetric->setText(0, "Metric");
	nodeInfoTree.nodeRank = new QTreeWidgetItem(nodeInfoTree.nodeMain);
	nodeInfoTree.nodeRank->setText(0, "Rank");
	nodeInfoTree.nodeGrounded = new QTreeWidgetItem(nodeInfoTree.nodeMain);
	nodeInfoTree.nodeGrounded->setText(0, "Grounded");
}

MainWindow::~MainWindow()
{
	rpl_tool_cleanup();
	delete ui;
}

void MainWindow::setNodeInfoTarget(const di_node_t* node, const di_dodag_t* dodag, const di_rpl_instance_t* rpl_instance) {
	char ipv6string[INET6_ADDRSTRLEN];

	if(!node)
		return;

	if(rpl_instance) {
		nodeInfoTree.rplInstanceId->setText(1, QString::number(rpl_instance_get_key(rpl_instance)->ref.rpl_instance));
		nodeInfoTree.rplInstanceModeOfOperation->setText(1, QString::number(rpl_instance_get_mop(rpl_instance)));
	} else {
		nodeInfoTree.rplInstanceId->setText(1, "");
		nodeInfoTree.rplInstanceModeOfOperation->setText(1, "");
	}

	if(dodag) {
		inet_ntop(AF_INET6, (const char*)&dodag_get_key(dodag)->ref.dodagid, ipv6string, INET6_ADDRSTRLEN);
		nodeInfoTree.dodagId->setText(1, ipv6string);

		nodeInfoTree.dodagVersion->setText(1, QString::number(dodag_get_key(dodag)->ref.version));

		inet_ntop(AF_INET6, (const char*)&dodag_get_prefix(dodag)->prefix, ipv6string, INET6_ADDRSTRLEN);
		nodeInfoTree.dodagPrefix->setText(1, QString(ipv6string) + "/" + QString::number(dodag_get_prefix(dodag)->length));

		nodeInfoTree.dodagConfigAuthEnabled->setText(1, (dodag_get_config(dodag)->auth_enabled? "Yes" : "No"));
		nodeInfoTree.dodagConfigDefaultLifetime->setText(1, QString::number(dodag_get_config(dodag)->default_lifetime));
		nodeInfoTree.dodagConfigDioIntervalMax->setText(1, QString::number(dodag_get_config(dodag)->dio_interval_max));
		nodeInfoTree.dodagConfigDioIntervalMin->setText(1, QString::number(dodag_get_config(dodag)->dio_interval_min));
		nodeInfoTree.dodagConfigDioRedundancyConstant->setText(1, QString::number(dodag_get_config(dodag)->dio_redundancy_constant));
		nodeInfoTree.dodagConfigLifetimeUnit->setText(1, QString::number(dodag_get_config(dodag)->lifetime_unit));
		nodeInfoTree.dodagConfigMaxRankIncrease->setText(1, QString::number(dodag_get_config(dodag)->max_rank_inc));
		nodeInfoTree.dodagConfigMinHopRankIncrease->setText(1, QString::number(dodag_get_config(dodag)->min_hop_rank_inc));
		nodeInfoTree.dodagConfigObjectiveFunction->setText(1, QString::number(dodag_get_config(dodag)->objective_function));
		nodeInfoTree.dodagConfigPathControlSize->setText(1, QString::number(dodag_get_config(dodag)->path_control_size));
	} else {
		nodeInfoTree.dodagId->setText(1, "");
		nodeInfoTree.dodagVersion->setText(1, "");
		nodeInfoTree.dodagPrefix->setText(1, "");

		nodeInfoTree.dodagConfigAuthEnabled->setText(1, "");
		nodeInfoTree.dodagConfigDefaultLifetime->setText(1, "");
		nodeInfoTree.dodagConfigDioIntervalMax->setText(1, "");
		nodeInfoTree.dodagConfigDioIntervalMin->setText(1, "");
		nodeInfoTree.dodagConfigDioRedundancyConstant->setText(1, "");
		nodeInfoTree.dodagConfigLifetimeUnit->setText(1, "");
		nodeInfoTree.dodagConfigMaxRankIncrease->setText(1, "");
		nodeInfoTree.dodagConfigMinHopRankIncrease->setText(1, "");
		nodeInfoTree.dodagConfigObjectiveFunction->setText(1, "");
		nodeInfoTree.dodagConfigPathControlSize->setText(1, "");
	}

	nodeInfoTree.nodeMacAddress->setText(1, QString::number(node_get_key(node)->ref.wpan_address, 16));

	inet_ntop(AF_INET6, (const char*)node_get_local_ip(node), ipv6string, INET6_ADDRSTRLEN);
	nodeInfoTree.nodeLocalIp->setText(1, ipv6string);

	inet_ntop(AF_INET6, (const char*)node_get_global_ip(node), ipv6string, INET6_ADDRSTRLEN);
	nodeInfoTree.nodeGlobalIp->setText(1, ipv6string);

	nodeInfoTree.nodeGrounded->setText(1, (node_get_grounded(node) ? "true" : "false"));
	const di_metric_t *metric = node_get_metric(node);
	QString metricValue;

	metricValue = QString::number(metric->value);
	if(metric->type) {
		metricValue += QString(" (") + metric->type->name + ": " + QString::number(metric_get_display_value(metric)) + ")";
	}
	nodeInfoTree.nodeMetric->setText(1, metricValue);
	nodeInfoTree.nodeRank->setText(1, QString::number(node_get_rank(node)));
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
