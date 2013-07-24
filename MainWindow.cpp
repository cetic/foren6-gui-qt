#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <rpl_packet_parser.h>
#include "rpl/rplNetworkInfoManager.h"
#include "RplDiagnosisTool.h"
#include "utlist.h"
#include "utlist.h"
#include <arpa/inet.h>

#include <QFileDialog>

#include <stdio.h>

MainWindow::MainWindow(RplDiagnosisTool *rplDiagnosisTool) :
	QMainWindow(0),
	ui(new Ui::MainWindow),
	rplDiagnosisTool(rplDiagnosisTool)
{
	ui->setupUi(this);
	ui->infoSplitter->setStretchFactor(0, 0);
	ui->infoSplitter->setStretchFactor(1, 1);

	ui->rplTreeView->setScene(rplDiagnosisTool->getScene());
	connect(ui->actionStart, SIGNAL(triggered()), this, SLOT(onStartSniffer()));
	connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(onStopSniffer()));
	connect(ui->actionOpenSniffer, SIGNAL(triggered()), this, SLOT(onOpenSniffer()));
	connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(onVersionSliderMove(int)));
	connect(ui->actionNewInformationWindow, SIGNAL(triggered()), this, SLOT(createNewInformationWindow()));


	{
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

		nodeInfoTree.routeMain = new QTreeWidgetItem(ui->rplNodeInfoTree);
		nodeInfoTree.routeMain->setText(0, "Routing table");
	}

	ui->rplNodeInfoTree->expandAll();
}

MainWindow::~MainWindow()
{
	rpl_tool_cleanup();
	delete ui;
}

void MainWindow::createNewInformationWindow() {
	InformationWidget *infoWidget;
	EventLog::Message *message;

	infoWidget = new InformationWidget(this);
	infoWidget->setFloating(true);
	infoWidget->show();

	connect(infoWidget, SIGNAL(setCurrentVersion(int)), this, SLOT(changeCurrentVersion(int)));
	connect(infoWidget, SIGNAL(destroyed(QObject*)), this, SLOT(onInformationWindowClosed(QObject*)));

	foreach(message, messages) {
		infoWidget->addMessage(message);
	}

	infoWidgets.append(infoWidget);
}

void MainWindow::onInformationWindowClosed(QObject *informationWindow) {

	infoWidgets.removeAll((InformationWidget*)informationWindow);
}

void MainWindow::addMessage(int version, const QString& type, const QString& message) {
	InformationWidget *infoWidget;

	EventLog::Message *newMsg = new EventLog::Message;
	printf("%d\t%d\t%s\t%s\n", rpldata_wsn_version_get_packet_count(version), version, type.toLatin1().constData(), message.toLatin1().constData());

	newMsg->version = version;
	newMsg->type = type;
	newMsg->message = message;

	messages.append(newMsg);

	foreach(infoWidget, infoWidgets) {
		infoWidget->addMessage(newMsg);
	}
}

void MainWindow::onVersionSliderMove(int value) {
	int version = qMin(value, ui->horizontalSlider->maximum()) + 1;

	if(version > ui->horizontalSlider->maximum())
		version = 0;  //realtime mode

	emit changeWsnVersion(version);
}

void MainWindow::changeCurrentVersion(int newVersion) {
	if(newVersion == 0)
		ui->horizontalSlider->setValue(ui->horizontalSlider->maximum());
	else ui->horizontalSlider->setValue(newVersion - 1);

	if(newVersion == 0)
		ui->versionLabel->setText(QString("realtime/%2").arg(ui->horizontalSlider->maximum()));
	else ui->versionLabel->setText(QString("%1/%2").arg(ui->horizontalSlider->value()+1).arg(ui->horizontalSlider->maximum()));

	emit changeWsnVersion(newVersion);
}

void MainWindow::updateVersionCount(int versionCount) {
	bool setRealtime;

	if(ui->horizontalSlider->value() == ui->horizontalSlider->maximum())
		setRealtime = true;
	else setRealtime = false;

	ui->horizontalSlider->setMaximum(versionCount);
	if(setRealtime)
		ui->horizontalSlider->setValue(versionCount);

	if(setRealtime)
		ui->versionLabel->setText(QString("realtime/%2").arg(versionCount));
	else ui->versionLabel->setText(QString("%1/%2").arg(ui->horizontalSlider->value()+1).arg(versionCount));
}

void MainWindow::setNodeInfoTarget(const di_node_t* node, const di_dodag_t* dodag, const di_rpl_instance_t* rpl_instance) {
	char ipv6string[INET6_ADDRSTRLEN];

	if(!node)
		return;

	if(rpl_instance) {
		nodeInfoTree.rplInstanceId->setText(1, QString::number(rpl_instance_get_key(rpl_instance)->ref.rpl_instance));
		nodeInfoTree.rplInstanceModeOfOperation->setText(1, QString::number(rpl_instance_get_mop(rpl_instance)));
	} else {
		if(dodag)
			nodeInfoTree.rplInstanceId->setText(1, QString::number(dodag_get_rpl_instance(dodag)->rpl_instance));
		else nodeInfoTree.rplInstanceId->setText(1, "");
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

	di_route_list_t route_table;
	di_route_el_t *route;
	route_table = node_get_routes(node);

	QList<QTreeWidgetItem *> routeChildren = nodeInfoTree.routeMain->takeChildren();
	QTreeWidgetItem *child;
	foreach(child, routeChildren)
		delete child;

	LL_FOREACH(route_table, route) {
		QStringList routeInfo;
		inet_ntop(AF_INET6, (const char*)&route->route_prefix.prefix, ipv6string, INET6_ADDRSTRLEN);
		routeInfo << QString(ipv6string) + "/" + QString::number(route->route_prefix.length) << QString::number(route->via_node, 16);
		nodeInfoTree.routeMain->addChild(new QTreeWidgetItem(routeInfo));
	}

}

void MainWindow::onStartSniffer() {
	rplDiagnosisTool->onStartSniffer();
}

void MainWindow::onStopSniffer() {
	rplDiagnosisTool->onStopSniffer();
}

void MainWindow::onOpenSniffer() {
	QString file = QFileDialog::getOpenFileName(this, "Choose interface file");
	if(!file.isNull()) {
		rplDiagnosisTool->openSnifferTarget(file);
	}
}
