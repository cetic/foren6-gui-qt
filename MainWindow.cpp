#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_About.h"
#include <rpl_packet_parser.h>
#include "rpl/rplNetworkInfoManager.h"
#include "utlist.h"
#include "utlist.h"
#include <arpa/inet.h>

#include "SnifferDialog.h"

#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QColor>

#include <stdio.h>
#include <math.h>

MainWindow *  MainWindow::thisInstance = 0;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
    analyzer_callbacks_t callbacks = {
      &onErrorEvent
    };

	ui->setupUi(this);
	ui->infoSplitter->setStretchFactor(0, 0);
	ui->infoSplitter->setStretchFactor(1, 1);

	packetWidget = 0;

	thisInstance = this;
    connect(this, SIGNAL(reportError(QString)), this, SLOT(onReportError(QString)), Qt::QueuedConnection);
    wsnManager = new rpl::NetworkInfoManager;

    QCoreApplication::setOrganizationName("CETIC");
    QCoreApplication::setOrganizationDomain("cetic.be");
    QCoreApplication::setApplicationName("Foren6");

	QSettings settings;
    settings.beginGroup("MainWindow");
    if (settings.contains("size")) {
        resize(settings.value("size", QSize(400, 400)).toSize());
    }
    if ( settings.contains("pos")) {
        move(settings.value("pos", QPoint(200, 200)).toPoint());
    }
    infoWidgetId = settings.value("info_widget_id", 0).toInt();
    if (settings.value("packet_widget", false).toBool()) {
        createNewPacketWindow();
    }
    int size = settings.beginReadArray("info_widgets");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        doCreateNewInformationWindow(settings.value("id").toString());
    }
    settings.endArray();

    restoreState(settings.value("windowState").toByteArray());
    settings.endGroup();

    rpl_tool_set_analyzer_callbacks(&callbacks);
	rpl_tool_init();

    captureStarted = false;
	snifferDialog = new SnifferDialog(this);
    bool hasActiveSniffers = snifferDialog->activeSniffersCount() > 0;
    ui->actionStart->setEnabled(hasActiveSniffers && ! captureStarted);
    ui->actionStop->setEnabled(hasActiveSniffers && captureStarted);

	ui->graphView->setNetworkManager(wsnManager);

	connect(ui->actionStart, SIGNAL(triggered()), this, SLOT(onStartSniffer()));
	connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(onStopSniffer()));
	connect(ui->actionOpenSnifferDialog, SIGNAL(triggered()), this, SLOT(onOpenSnifferDialog()));
	connect(ui->versionSlider, SIGNAL(changeWsnVersion(int)), this, SIGNAL(changeWsnVersion(int)));
    connect(ui->actionLoadBackground, SIGNAL(triggered()), wsnManager, SLOT(onLoadBackground()));
    connect(ui->actionClearBackground, SIGNAL(triggered()), wsnManager, SLOT(onClearBackground()));
    connect(ui->actionLoadLayout, SIGNAL(triggered()), wsnManager, SLOT(onLoadLayout()));
    connect(ui->actionSaveLayout, SIGNAL(triggered()), wsnManager, SLOT(onSaveLayout()));
    connect(ui->actionClearLayout, SIGNAL(triggered()), wsnManager, SLOT(onClearLayout()));
	connect(ui->actionNewInformationWindow, SIGNAL(triggered()), this, SLOT(createNewInformationWindow()));
    connect(ui->actionNewPacketWindow, SIGNAL(triggered()), this, SLOT(createNewPacketWindow()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(createNewAboutWindow()));
	connect(ui->actionToggleNodeMovement, SIGNAL(triggered()), wsnManager->scene(), SLOT(toggleNodeMovement()));
	connect(ui->actionClear, SIGNAL(triggered()), this, SLOT(onClear()));
    connect(ui->actionToggleNodeInfo, SIGNAL(triggered()), wsnManager->scene(), SLOT(toggleNodeInfo()));

	connect(this, SIGNAL(changeWsnVersion(int)), wsnManager, SLOT(useVersion(int)));
	connect(this, SIGNAL(toggleNodeMovement()), wsnManager->scene(), SLOT(toggleNodeMovement()));

	connect(wsnManager, SIGNAL(nodeUpdateSelected(const di_node_t*,const di_dodag_t*,const di_rpl_instance_t*)), this, SLOT(setTargetNodeInfo(const di_node_t*,const di_dodag_t*,const di_rpl_instance_t*)));
	connect(wsnManager, SIGNAL(updateVersionCount(int)), ui->versionSlider, SLOT(onUpdateVersionCount(int)));
	connect(wsnManager, SIGNAL(logMessage(rpl::Event*)), this, SLOT(addMessage(rpl::Event*)));
	connect(wsnManager, SIGNAL(clearMessages()), this, SLOT(clearMessages()));
    connect(wsnManager, SIGNAL(layoutChanged(QString)), this, SLOT(layoutChanged(QString)));

	//QStatusBar can not be edited in QtCreator
	statusBar()->addWidget(new QLabel("Layout: "));
    statusBar()->addWidget(&layoutName);

    //Load layout from global preferences
    wsnManager->setLayout(settings.value("layout", QString()).toString());

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
		nodeInfoTree.dodagConfigDioIntervalMax->setText(0, "DIO Interval Doublings");
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
		nodeInfoTree.nodeTraffic = new QTreeWidgetItem(nodeInfoTree.nodeMain);
		nodeInfoTree.nodeTraffic->setText(0, "Transmited Packets");
		nodeInfoTree.nodeMaxDaoInterval = new QTreeWidgetItem(nodeInfoTree.nodeMain);
		nodeInfoTree.nodeMaxDaoInterval->setText(0, "Max DAO Interval");
		nodeInfoTree.nodeMaxDioInterval = new QTreeWidgetItem(nodeInfoTree.nodeMain);
		nodeInfoTree.nodeMaxDioInterval->setText(0, "Max DIO Interval");
		nodeInfoTree.nodeLastDtsn = new QTreeWidgetItem(nodeInfoTree.nodeMain);
		nodeInfoTree.nodeLastDtsn->setText(0, "Last DTSN");
		nodeInfoTree.nodeLastDaoSeq = new QTreeWidgetItem(nodeInfoTree.nodeMain);
		nodeInfoTree.nodeLastDaoSeq->setText(0, "Last DAO Sequence");
		nodeInfoTree.nodeUpwardRankErrorCount = new QTreeWidgetItem(nodeInfoTree.nodeMain);
		nodeInfoTree.nodeUpwardRankErrorCount->setText(0, "Upward rank errors");
		nodeInfoTree.nodeDownwardRankErrorCount = new QTreeWidgetItem(nodeInfoTree.nodeMain);
		nodeInfoTree.nodeDownwardRankErrorCount->setText(0, "Downward rank errors");
		nodeInfoTree.nodeRouteLoopErrorCount = new QTreeWidgetItem(nodeInfoTree.nodeMain);
		nodeInfoTree.nodeRouteLoopErrorCount->setText(0, "Route loop errors");
		nodeInfoTree.nodeIpMismatchErrorCount = new QTreeWidgetItem(nodeInfoTree.nodeMain);
		nodeInfoTree.nodeIpMismatchErrorCount->setText(0, "IP mismatch errors");
		nodeInfoTree.nodeDodagMismatchErrorCount = new QTreeWidgetItem(nodeInfoTree.nodeMain);
		nodeInfoTree.nodeDodagMismatchErrorCount->setText(0, "Dodag mismatch errors");

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

void MainWindow::onErrorEvent(char const * errorMessage) {
    if ( !thisInstance ) return;
    thisInstance->emit reportError(errorMessage);
}

void MainWindow::onReportError(QString errorMessage) {
    QMessageBox messageBox;
    messageBox.critical(0, "Error", errorMessage);
    QCoreApplication::exit(1);
}

void MainWindow::createNewInformationWindow() {
    doCreateNewInformationWindow();
}

void MainWindow::doCreateNewInformationWindow(QString name) {
    InformationWidget *infoWidget;
    rpl::Event *message;
    infoWidget = new InformationWidget(this);
	infoWidget->setFloating(true);
    infoWidget->move(this->pos()); /* Place at the same position as main window to avoid multi-screen glitches */
	infoWidget->show();
    if ( name.isEmpty() ) {
        infoWidget->setObjectName(QString("InformationWidget-") + QString::number(infoWidgetId));
        infoWidgetId++;
    } else {
        infoWidget->setObjectName(name);
    }

    connect(infoWidget, SIGNAL(setCurrentVersion(int)), ui->versionSlider, SLOT(onChangeCurrentVersion(int)));
    connect(infoWidget, SIGNAL(destroyed(QObject*)), this, SLOT(onInformationWindowClosed(QObject*)));
    connect(infoWidget, SIGNAL(messageSelected(rpl::Event*)), this, SLOT(messageSelected(rpl::Event*)));
    connect(wsnManager, SIGNAL(clearMessages()), infoWidget, SLOT(clearMessages()));

    foreach(message, messages) {
        infoWidget->addMessage(message);
    }

    infoWidgets.append(infoWidget);
}

void MainWindow::createNewPacketWindow() {
    if (packetWidget) return;
    packetWidget = new PacketWidget(this);
    packetWidget->setFloating(true);
    packetWidget->show();
    connect(packetWidget, SIGNAL(destroyed(QObject*)), this, SLOT(onPacketWindowClosed(QObject*)));
}

void MainWindow::createNewAboutWindow() {

    QDialog window;
    Ui::About ui;
    ui.setupUi(&window);
    window.exec();
}

void MainWindow::onInformationWindowClosed(QObject *informationWindow) {
	infoWidgets.removeAll((InformationWidget*)informationWindow);
}

void MainWindow::onPacketWindowClosed(QObject *packetWindow) {
	packetWidget = 0;
}

void MainWindow::addMessage(rpl::Event *event) {
	InformationWidget *infoWidget;

	messages.append(event);

	foreach(infoWidget, infoWidgets) {
		infoWidget->addMessage(event);
	}
}

void MainWindow::clearMessages() {
	rpl::Event *message;

	foreach(message, messages) {
		delete message;
	}
	messages.clear();

}

void MainWindow::messageSelected(rpl::Event * event) {
    if ( ! packetWidget) return;
    packetWidget->showPacket(event);
}

void MainWindow::layoutChanged(QString layoutFile) {
    layoutName.setText(!layoutFile.isEmpty() ? layoutFile : "-");
    //Save layout in global preferences
    QSettings settings;
    settings.setValue("layout", layoutFile);
}

void MainWindow::setDeltaColor(QTreeWidgetItem * widget, int delta, QColor color) {
    widget->setForeground(1, QBrush(delta ? QBrush(color) : QBrush(QColor(0, 0, 0))));
}

void MainWindow::setTargetNodeInfo(const di_node_t* node, const di_dodag_t* dodag, const di_rpl_instance_t* rpl_instance) {
	char ipv6string[INET6_ADDRSTRLEN];

	if(!node) {
		clearTargetNodeInfo();
		return;
	}

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
    setDeltaColor( nodeInfoTree.nodeLocalIp, node_get_local_address_delta(node), QColor(Qt::blue));

	inet_ntop(AF_INET6, (const char*)node_get_global_ip(node), ipv6string, INET6_ADDRSTRLEN);
	nodeInfoTree.nodeGlobalIp->setText(1, ipv6string);
    setDeltaColor( nodeInfoTree.nodeGlobalIp, node_get_global_address_delta(node), QColor(Qt::blue));

	nodeInfoTree.nodeGrounded->setText(1, (node_get_grounded(node) ? "true" : "false"));
    setDeltaColor( nodeInfoTree.nodeGrounded, node_get_grounded_delta(node), QColor(Qt::blue));

    const di_metric_t *metric = node_get_metric(node);
	QString metricValue;

	metricValue = QString::number(metric->value);
	if(metric->type) {
		metricValue += QString(" (") + metric->type->name + ": " + QString::number(metric_get_display_value(metric)) + ")";
	}
	nodeInfoTree.nodeMetric->setText(1, metricValue);
    setDeltaColor( nodeInfoTree.nodeMetric, node_get_metric_delta(node), QColor(Qt::blue));
	nodeInfoTree.nodeRank->setText(1, QString::number(node_get_rank(node)));
    setDeltaColor( nodeInfoTree.nodeRank, node_get_rank_delta(node), QColor(Qt::blue));

	nodeInfoTree.nodeTraffic->setText(1, QString::number(node_get_packet_count(node)));
    setDeltaColor( nodeInfoTree.nodeTraffic, node_get_packet_count_delta(node), QColor(Qt::blue));

	nodeInfoTree.nodeMaxDaoInterval->setText(1, QString::number(node_get_max_dao_interval(node)) + " sec");
    setDeltaColor( nodeInfoTree.nodeMaxDaoInterval, node_get_max_dao_interval_delta(node), QColor(Qt::blue));
	nodeInfoTree.nodeMaxDioInterval->setText(1, QString::number(node_get_max_dio_interval(node)) + " sec");
    setDeltaColor( nodeInfoTree.nodeMaxDioInterval, node_get_max_dio_interval_delta(node), QColor(Qt::blue));
	nodeInfoTree.nodeLastDtsn->setText(1, QString::number(node_get_dtsn(node)));
    setDeltaColor( nodeInfoTree.nodeLastDtsn, node_get_latest_dtsn_delta(node), QColor(Qt::blue));
	nodeInfoTree.nodeLastDaoSeq->setText(1, QString::number(node_get_dao_seq(node)));
    setDeltaColor( nodeInfoTree.nodeLastDaoSeq, node_get_latest_dao_sequence_delta(node), QColor(Qt::blue));

    nodeInfoTree.nodeUpwardRankErrorCount->setText(1, QString::number(node_get_upward_error_count(node)));
    setDeltaColor( nodeInfoTree.nodeUpwardRankErrorCount, node_get_upward_error_delta(node), QColor(Qt::red));
	nodeInfoTree.nodeDownwardRankErrorCount->setText(1, QString::number(node_get_downward_error_count(node)));
    setDeltaColor( nodeInfoTree.nodeDownwardRankErrorCount, node_get_downward_error_delta(node), QColor(Qt::red));
	nodeInfoTree.nodeRouteLoopErrorCount->setText(1, QString::number(node_get_route_error_count(node)));
	setDeltaColor( nodeInfoTree.nodeRouteLoopErrorCount, node_get_route_error_delta(node), QColor(Qt::red));
	nodeInfoTree.nodeIpMismatchErrorCount->setText(1, QString::number(node_get_ip_mismatch_error_count(node)));
    setDeltaColor( nodeInfoTree.nodeIpMismatchErrorCount, node_get_ip_mismatch_error_delta(node), QColor(Qt::red));
	nodeInfoTree.nodeDodagMismatchErrorCount->setText(1, QString::number(node_get_dodag_mismatch_error_count(node)));
    setDeltaColor( nodeInfoTree.nodeDodagMismatchErrorCount, node_get_dodag_mismatch_error_delta(node), QColor(Qt::red));

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
	nodeInfoTree.routeMain->setForeground(0, QBrush(node_get_routes_delta(node) ? QBrush(Qt::blue) : QBrush(Qt::black)));
}

void MainWindow::clearTargetNodeInfo() {
	nodeInfoTree.rplInstanceId->setText(1, "");
	nodeInfoTree.rplInstanceModeOfOperation->setText(1, "");

	nodeInfoTree.dodagVersion->setText(1, "");
	nodeInfoTree.dodagId->setText(1, "");
	nodeInfoTree.dodagPrefix->setText(1, "");
	nodeInfoTree.dodagConfigAuthEnabled->setText(1, "");
	nodeInfoTree.dodagConfigPathControlSize->setText(1, "");
	nodeInfoTree.dodagConfigDioIntervalMin->setText(1, "");
	nodeInfoTree.dodagConfigDioIntervalMax->setText(1, "");
	nodeInfoTree.dodagConfigDioRedundancyConstant->setText(1, "");
	nodeInfoTree.dodagConfigMaxRankIncrease->setText(1, "");
	nodeInfoTree.dodagConfigMinHopRankIncrease->setText(1, "");
	nodeInfoTree.dodagConfigDefaultLifetime->setText(1, "");
	nodeInfoTree.dodagConfigLifetimeUnit->setText(1, "");
	nodeInfoTree.dodagConfigObjectiveFunction->setText(1, "");

	nodeInfoTree.nodeMacAddress->setText(1, "");
	nodeInfoTree.nodeLocalIp->setText(1, "");
	nodeInfoTree.nodeGlobalIp->setText(1, "");
	nodeInfoTree.nodeMetric->setText(1, "");
	nodeInfoTree.nodeRank->setText(1, "");
	nodeInfoTree.nodeGrounded->setText(1, "");
	nodeInfoTree.nodeTraffic->setText(1, "");
	nodeInfoTree.nodeMaxDaoInterval->setText(1, "");
	nodeInfoTree.nodeMaxDioInterval->setText(1, "");
	nodeInfoTree.nodeLastDtsn->setText(1, "");
	nodeInfoTree.nodeLastDaoSeq->setText(1, "");
	nodeInfoTree.nodeUpwardRankErrorCount->setText(1, "");
	nodeInfoTree.nodeDownwardRankErrorCount->setText(1, "");
	nodeInfoTree.nodeRouteLoopErrorCount->setText(1, "");
	nodeInfoTree.nodeDodagMismatchErrorCount->setText(1, "");
	nodeInfoTree.nodeIpMismatchErrorCount->setText(1, "");

	QList<QTreeWidgetItem *> routeChildren = nodeInfoTree.routeMain->takeChildren();
	QTreeWidgetItem *child;
	foreach(child, routeChildren)
		delete child;
}

void MainWindow::onStartSniffer() {
	snifferDialog->onStartSniffer();
	ui->actionStart->setEnabled(false);
	ui->actionStop->setEnabled(true);
	ui->actionOpenSnifferDialog->setEnabled(false);
	captureStarted=true;
}

void MainWindow::onStopSniffer() {
	snifferDialog->onStopSniffer();
    ui->actionStart->setEnabled(true);
    ui->actionStop->setEnabled(false);
    ui->actionOpenSnifferDialog->setEnabled(true);
    captureStarted=false;
}

void MainWindow::onOpenSnifferDialog() {
    snifferDialog->exec();
    bool hasActiveSniffers = snifferDialog->activeSniffersCount() > 0;
    ui->actionStart->setEnabled(hasActiveSniffers && ! captureStarted);
    ui->actionStop->setEnabled(hasActiveSniffers && captureStarted);
}

void MainWindow::onClear() {
	rpldata_clear();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("info_widget_id", infoWidgetId);
    settings.setValue("packet_widget", packetWidget != 0);
    //Recommended way of clearing up an array in QSettings...
    settings.beginGroup("info_widgets");
    settings.remove("");
    settings.endGroup();
    settings.beginWriteArray("info_widgets");
    for (int i = 0; i < infoWidgets.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("id", infoWidgets.at(i)->objectName());
    }
    settings.endArray();

    settings.setValue("windowState", saveState());
    settings.endGroup();
    event->accept();
}
