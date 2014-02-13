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
 *         Foren6 Main Window Management
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_About.h"
#include "SettingsDialog.h"
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

MainWindow *
    MainWindow::thisInstance = 0;

MainWindow::MainWindow(QWidget * parent):
QMainWindow(parent), ui(new Ui::MainWindow)
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
    if(settings.contains("size")) {
        resize(settings.value("size", QSize(400, 400)).toSize());
    }
    if(settings.contains("pos")) {
        move(settings.value("pos", QPoint(200, 200)).toPoint());
    }
    infoWidgetId = settings.value("info_widget_id", 0).toInt();
    if(settings.value("packet_widget", false).toBool()) {
        createNewPacketWindow();
    }
    int size = settings.beginReadArray("info_widgets");

    for(int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        doCreateNewInformationWindow(settings.value("id").toString());
    }
    settings.endArray();

    restoreState(settings.value("windowState").toByteArray());
    settings.endGroup();

    ui->actionToggleNodeMovement->setChecked(settings.value("nodeMovement", false).toBool());
    wsnManager->scene()->setNodeMovement(!ui->actionToggleNodeMovement->isChecked());
    ui->actionToggleNodeInfo->setChecked(settings.value("nodeInfo", true).toBool());
    wsnManager->scene()->setNodeInfo(ui->actionToggleNodeInfo->isChecked());
    ui->actionLinkUnlinkDialogs->setChecked(settings.value("linkDialogs", true).toBool());
    wsnManager->setLinkDialogs(ui->actionLinkUnlinkDialogs->isChecked());

    rpl_tool_set_analyzer_callbacks(&callbacks);
    rpl_tool_init();

    captureStarted = false;
    snifferDialog = new SnifferDialog(this);
    bool hasActiveSniffers = snifferDialog->activeSniffersCount() > 0;

    ui->actionStart->setEnabled(hasActiveSniffers && !captureStarted);
    ui->actionStop->setEnabled(hasActiveSniffers && captureStarted);

    settingsDialog = new SettingsDialog(this);

    rpl_tool_start();

    ui->graphView->setNetworkManager(wsnManager);

    connect(ui->actionStart, SIGNAL(triggered()), this, SLOT(onStartSniffer()));
    connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(onStopSniffer()));
    connect(ui->actionOpenSnifferDialog, SIGNAL(triggered()), this, SLOT(onOpenSnifferDialog()));
    connect(ui->versionSlider, SIGNAL(changeWsnVersion(int)), this, SLOT(onSliderVersionChanged(int)));
    connect(ui->versionSlider, SIGNAL(changeWsnVersion(int)), wsnManager, SLOT(useVersion(int)));

    connect(ui->actionLoadBackground, SIGNAL(triggered()), wsnManager, SLOT(onLoadBackground()));
    connect(ui->actionClearBackground, SIGNAL(triggered()), wsnManager, SLOT(onClearBackground()));
    connect(ui->actionLoadLayout, SIGNAL(triggered()), wsnManager, SLOT(onLoadLayout()));
    connect(ui->actionSaveLayout, SIGNAL(triggered()), wsnManager, SLOT(onSaveLayout()));
    connect(ui->actionClearLayout, SIGNAL(triggered()), wsnManager, SLOT(onClearLayout()));
    connect(ui->actionClearAllErrors, SIGNAL(triggered()), this, SLOT(clearAllErrors()));
    connect(ui->actionClearDeprecatedLinks, SIGNAL(triggered()), this, SLOT(clearDeprecatedLinks()));
    connect(ui->actionNewInformationWindow, SIGNAL(triggered()), this, SLOT(createNewInformationWindow()));
    connect(ui->actionNewPacketWindow, SIGNAL(triggered()), this, SLOT(createNewPacketWindow()));
    connect(ui->actionClosePacketWindow, SIGNAL(triggered()), this, SLOT(closePacketWindow()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(createNewAboutWindow()));
    connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(editSettings()));
    connect(ui->actionToggleNodeMovement, SIGNAL(triggered()), this, SLOT(onToggleNodeMovement()));
    connect(ui->actionClear, SIGNAL(triggered()), this, SLOT(onClear()));
    connect(ui->actionToggleNodeInfo, SIGNAL(triggered()), this, SLOT(onToggleNodeInfo()));
    connect(ui->actionLinkUnlinkDialogs, SIGNAL(triggered(bool)), this, SLOT(onToggleLinkDialogs(bool)));

    connect(this, SIGNAL(changeWsnVersion(int)), wsnManager, SLOT(useVersion(int)));

    connect(wsnManager, SIGNAL(nodeUpdateSelected(const di_node_t *, const di_dodag_t *, const di_rpl_instance_t *)), this,
            SLOT(setTargetNodeInfo(const di_node_t *, const di_dodag_t *, const di_rpl_instance_t *)));
    connect(wsnManager, SIGNAL(updateVersionCount(int)), ui->versionSlider, SLOT(onUpdateVersionCount(int)));
    connect(wsnManager, SIGNAL(timeTick(int)), ui->versionSlider, SLOT(onTimeTick(int)));

    connect(wsnManager, SIGNAL(logMessage(rpl::Event *)), this, SLOT(addMessage(rpl::Event *)));
    connect(wsnManager, SIGNAL(clearMessages()), this, SLOT(clearMessages()));
    connect(wsnManager, SIGNAL(layoutChanged(QString)), this, SLOT(layoutChanged(QString)));

    //QStatusBar can not be edited in QtCreator
    statusBar()->addWidget(new QLabel("Layout: "));
    statusBar()->addWidget(&layoutName);

    //Load layout from global preferences
    wsnManager->setLayout(settings.value("layout", QString()).toString());

    {
        nodeInfoTree.linkMain = new QTreeWidgetItem(ui->rplNodeInfoTree);
        nodeInfoTree.linkMain->setText(0, "802.15.4");
        nodeInfoTree.nodeMacAddress = new QTreeWidgetItem(nodeInfoTree.linkMain);
        nodeInfoTree.nodeMacAddress->setText(0, "MAC Address");
        nodeInfoTree.ipv6Main = new QTreeWidgetItem(ui->rplNodeInfoTree);
        nodeInfoTree.ipv6Main->setText(0, "IPv6");
        nodeInfoTree.nodeLocalIp = new QTreeWidgetItem(nodeInfoTree.ipv6Main);
        nodeInfoTree.nodeLocalIp->setText(0, "Link-Local IP");
        nodeInfoTree.nodeGlobalIp = new QTreeWidgetItem(nodeInfoTree.ipv6Main);
        nodeInfoTree.nodeGlobalIp->setText(0, "Global IP");

        nodeInfoTree.rplInstanceMain = new QTreeWidgetItem(ui->rplNodeInfoTree);
        nodeInfoTree.rplInstanceMain->setText(0, "RPL configuration");
        nodeInfoTree.rplInstanceIdMain = new QTreeWidgetItem(nodeInfoTree.rplInstanceMain);
        nodeInfoTree.rplInstanceIdMain->setText(0, "Instance ID");

        nodeInfoTree.dodagId = new QTreeWidgetItem(nodeInfoTree.rplInstanceIdMain);
        nodeInfoTree.dodagId->setText(0, "DODAG ID");
        nodeInfoTree.dodagVersion = new QTreeWidgetItem(nodeInfoTree.rplInstanceIdMain);
        nodeInfoTree.dodagVersion->setText(0, "Version");
        nodeInfoTree.dodagModeOfOperation = new QTreeWidgetItem(nodeInfoTree.rplInstanceIdMain);
        nodeInfoTree.dodagModeOfOperation->setText(0, "Mode of Operation");
        nodeInfoTree.dodagPrefix = new QTreeWidgetItem(nodeInfoTree.rplInstanceIdMain);
        nodeInfoTree.dodagPrefix->setText(0, "Prefix");
        nodeInfoTree.dodagPrefixOnLink = new QTreeWidgetItem(nodeInfoTree.dodagPrefix);
        nodeInfoTree.dodagPrefixOnLink->setText(0, "On-link");
        nodeInfoTree.dodagPrefixAutoconf = new QTreeWidgetItem(nodeInfoTree.dodagPrefix);
        nodeInfoTree.dodagPrefixAutoconf->setText(0, "Autoconf");
        nodeInfoTree.dodagPrefixRouterAddress = new QTreeWidgetItem(nodeInfoTree.dodagPrefix);
        nodeInfoTree.dodagPrefixRouterAddress->setText(0, "Router address");
        nodeInfoTree.dodagPrefixValidLifetime = new QTreeWidgetItem(nodeInfoTree.dodagPrefix);
        nodeInfoTree.dodagPrefixValidLifetime->setText(0, "Valid lifetime");
        nodeInfoTree.dodagPrefixPreferredLifetime = new QTreeWidgetItem(nodeInfoTree.dodagPrefix);
        nodeInfoTree.dodagPrefixPreferredLifetime->setText(0, "Preferred lifetime");
        nodeInfoTree.dodagConfig = new QTreeWidgetItem(nodeInfoTree.rplInstanceIdMain);

        nodeInfoTree.dodagConfig->setText(0, "Configuration");
        nodeInfoTree.dodagConfigAuthEnabled = new QTreeWidgetItem(nodeInfoTree.dodagConfig);
        nodeInfoTree.dodagConfigAuthEnabled->setText(0, "Use Authentication");
        nodeInfoTree.dodagConfigPathControlSize = new QTreeWidgetItem(nodeInfoTree.dodagConfig);
        nodeInfoTree.dodagConfigPathControlSize->setText(0, "Path Control Size");
        nodeInfoTree.dodagConfigDioIntervalMin = new QTreeWidgetItem(nodeInfoTree.dodagConfig);
        nodeInfoTree.dodagConfigDioIntervalMin->setText(0, "DIO Interval Min");
        nodeInfoTree.dodagConfigDioIntervalMax = new QTreeWidgetItem(nodeInfoTree.dodagConfig);
        nodeInfoTree.dodagConfigDioIntervalMax->setText(0, "DIO Interval Doublings");
        nodeInfoTree.dodagConfigDioRedundancyConstant = new QTreeWidgetItem(nodeInfoTree.dodagConfig);
        nodeInfoTree.dodagConfigDioRedundancyConstant->setText(0, "DIO Redundancy Constant");
        nodeInfoTree.dodagConfigMaxRankIncrease = new QTreeWidgetItem(nodeInfoTree.dodagConfig);
        nodeInfoTree.dodagConfigMaxRankIncrease->setText(0, "Max Rank Increase");
        nodeInfoTree.dodagConfigMinHopRankIncrease = new QTreeWidgetItem(nodeInfoTree.dodagConfig);
        nodeInfoTree.dodagConfigMinHopRankIncrease->setText(0, "Min Hop Rank Increase");
        nodeInfoTree.dodagConfigDefaultLifetime = new QTreeWidgetItem(nodeInfoTree.dodagConfig);
        nodeInfoTree.dodagConfigDefaultLifetime->setText(0, "Defaut Lifetime");
        nodeInfoTree.dodagConfigLifetimeUnit = new QTreeWidgetItem(nodeInfoTree.dodagConfig);
        nodeInfoTree.dodagConfigLifetimeUnit->setText(0, "Lifetime Unit");
        nodeInfoTree.dodagConfigObjectiveFunction = new QTreeWidgetItem(nodeInfoTree.dodagConfig);
        nodeInfoTree.dodagConfigObjectiveFunction->setText(0, "Objective Function");

        nodeInfoTree.rplDataMain = new QTreeWidgetItem(ui->rplNodeInfoTree);
        nodeInfoTree.rplDataMain->setText(0, "RPL operational data");
        nodeInfoTree.rplDodagDataMain = new QTreeWidgetItem(nodeInfoTree.rplDataMain);
        nodeInfoTree.rplDodagDataMain->setText(0, "Instance ID");
        nodeInfoTree.nodeMetric = new QTreeWidgetItem(nodeInfoTree.rplDodagDataMain);
        nodeInfoTree.nodeMetric->setText(0, "Metric");
        nodeInfoTree.nodeRank = new QTreeWidgetItem(nodeInfoTree.rplDodagDataMain);
        nodeInfoTree.nodeRank->setText(0, "Rank");
        nodeInfoTree.nodeGrounded = new QTreeWidgetItem(nodeInfoTree.rplDodagDataMain);
        nodeInfoTree.nodeGrounded->setText(0, "Grounded");
        nodeInfoTree.nodePreference = new QTreeWidgetItem(nodeInfoTree.rplDodagDataMain);
        nodeInfoTree.nodePreference->setText(0, "Preference");
        nodeInfoTree.nodeLastDtsn = new QTreeWidgetItem(nodeInfoTree.rplDodagDataMain);
        nodeInfoTree.nodeLastDtsn->setText(0, "Last DTSN");
        nodeInfoTree.nodeLastDaoSeq = new QTreeWidgetItem(nodeInfoTree.rplDodagDataMain);
        nodeInfoTree.nodeLastDaoSeq->setText(0, "Last DAO Sequence");

        nodeInfoTree.statisticsMain = new QTreeWidgetItem(ui->rplNodeInfoTree);
        nodeInfoTree.statisticsMain->setText(0, "Statistics");
        nodeInfoTree.nodeTraffic = new QTreeWidgetItem(nodeInfoTree.statisticsMain);
        nodeInfoTree.nodeTraffic->setText(0, "Transmited Packets");
        nodeInfoTree.nodeMaxDaoInterval = new QTreeWidgetItem(nodeInfoTree.statisticsMain);
        nodeInfoTree.nodeMaxDaoInterval->setText(0, "Max DAO Interval");
        nodeInfoTree.nodeMaxDioInterval = new QTreeWidgetItem(nodeInfoTree.statisticsMain);
        nodeInfoTree.nodeMaxDioInterval->setText(0, "Max DIO Interval");
        nodeInfoTree.nodeDis = new QTreeWidgetItem(nodeInfoTree.statisticsMain);
        nodeInfoTree.nodeDis->setText(0, "DIS sent");
        nodeInfoTree.nodeDio = new QTreeWidgetItem(nodeInfoTree.statisticsMain);
        nodeInfoTree.nodeDio->setText(0, "DIO sent");
        nodeInfoTree.nodeDao = new QTreeWidgetItem(nodeInfoTree.statisticsMain);
        nodeInfoTree.nodeDao->setText(0, "DAO sent");

        nodeInfoTree.errorsMain = new QTreeWidgetItem(ui->rplNodeInfoTree);
        nodeInfoTree.errorsMain->setText(0, "Errors");
        nodeInfoTree.nodeInvalidIp = new QTreeWidgetItem(nodeInfoTree.errorsMain);
        nodeInfoTree.nodeInvalidIp->setText(0, "Invalid IP");
        nodeInfoTree.nodeInvalidPrefix = new QTreeWidgetItem(nodeInfoTree.errorsMain);
        nodeInfoTree.nodeInvalidPrefix->setText(0, "Invalid prefix");
        nodeInfoTree.nodeRankErrorCount = new QTreeWidgetItem(nodeInfoTree.errorsMain);
        nodeInfoTree.nodeRankErrorCount->setText(0, "Rank errors");
        nodeInfoTree.nodeForwardErrorCount = new QTreeWidgetItem(nodeInfoTree.errorsMain);
        nodeInfoTree.nodeForwardErrorCount->setText(0, "Forward errors");
        nodeInfoTree.nodeUpwardRankErrorCount = new QTreeWidgetItem(nodeInfoTree.errorsMain);
        nodeInfoTree.nodeUpwardRankErrorCount->setText(0, "Upward rank errors");
        nodeInfoTree.nodeDownwardRankErrorCount = new QTreeWidgetItem(nodeInfoTree.errorsMain);
        nodeInfoTree.nodeDownwardRankErrorCount->setText(0, "Downward rank errors");
        nodeInfoTree.nodeRouteLoopErrorCount = new QTreeWidgetItem(nodeInfoTree.errorsMain);
        nodeInfoTree.nodeRouteLoopErrorCount->setText(0, "Route loop errors");
        nodeInfoTree.nodeIpMismatchErrorCount = new QTreeWidgetItem(nodeInfoTree.errorsMain);
        nodeInfoTree.nodeIpMismatchErrorCount->setText(0, "DAO IP mismatch errors");
        nodeInfoTree.nodeDodagMismatchErrorCount = new QTreeWidgetItem(nodeInfoTree.errorsMain);
        nodeInfoTree.nodeDodagMismatchErrorCount->setText(0, "Dodag mismatch errors");
        nodeInfoTree.nodeDodagConfigMismatchErrorCount = new QTreeWidgetItem(nodeInfoTree.errorsMain);
        nodeInfoTree.nodeDodagConfigMismatchErrorCount->setText(0, "Dodag config mismatch errors");

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

void
MainWindow::onErrorEvent(char const *errorMessage)
{
    if(!thisInstance)
        return;
    thisInstance->emit reportError(errorMessage);
}

void
MainWindow::onReportError(QString errorMessage)
{
    QMessageBox messageBox;

    messageBox.critical(0, "Error", errorMessage);
    QCoreApplication::exit(1);
}

void
MainWindow::createNewInformationWindow()
{
    doCreateNewInformationWindow();
}

void
MainWindow::doCreateNewInformationWindow(QString name)
{
    InformationWidget *infoWidget;

    rpl::Event * message;
    infoWidget = new InformationWidget(this);
    infoWidget->setFloating(true);
    infoWidget->move(this->pos());      /* Place at the same position as main window to avoid multi-screen glitches */
    infoWidget->show();
    if(name.isEmpty()) {
        infoWidget->setObjectName(QString("InformationWidget-") + QString::number(infoWidgetId));
        infoWidgetId++;
    } else {
        infoWidget->setObjectName(name);
    }

    connect(infoWidget, SIGNAL(setCurrentVersion(int)), ui->versionSlider, SLOT(onChangeCurrentVersion(int)));

    connect(infoWidget, SIGNAL(destroyed(QObject *)), this, SLOT(onInformationWindowClosed(QObject *)));
    connect(infoWidget, SIGNAL(messageSelected(rpl::Event *)), this, SLOT(messageSelected(rpl::Event *)));
    connect(this, SIGNAL(changeWsnVersionInfo(int)), infoWidget, SLOT(onChangeCurrentVersion(int)));

    connect(wsnManager, SIGNAL(clearMessages()), infoWidget, SLOT(clearMessages()));
    connect(ui->actionLinkUnlinkDialogs, SIGNAL(triggered(bool)), infoWidget, SLOT(onToggleLinkDialogs(bool)));

    foreach(message, messages) {
        infoWidget->addMessage(message);
    }

    infoWidgets.append(infoWidget);
}

void
MainWindow::createNewPacketWindow()
{
    if(packetWidget)
        return;
    packetWidget = new PacketWidget(this);
    packetWidget->setFloating(true);
    packetWidget->show();
    packetWidget->move(this->pos());    /* Place at the same position as main window to avoid multi-screen glitches */
    connect(packetWidget, SIGNAL(destroyed(QObject *)), this, SLOT(onPacketWindowClosed(QObject *)));
    ui->actionNewPacketWindow->setEnabled(false);
    ui->actionClosePacketWindow->setEnabled(true);
}

void
MainWindow::closePacketWindow()
{
    if(packetWidget) {
        packetWidget->close();
        ui->actionNewPacketWindow->setEnabled(true);
        ui->actionClosePacketWindow->setEnabled(false);
    }
}

void
MainWindow::createNewAboutWindow()
{

    QDialog window;

    Ui::About ui;
    ui.setupUi(&window);
    window.exec();
}

void
MainWindow::editSettings()
{
    if(settingsDialog->exec()) {
        settingsDialog->applySettings();
    } else {
        settingsDialog->restoreSettings();
    }
}


void
MainWindow::onInformationWindowClosed(QObject * informationWindow)
{
    infoWidgets.removeAll((InformationWidget *) informationWindow);
}

void
MainWindow::onPacketWindowClosed(QObject * packetWindow)
{
    packetWidget = 0;
    ui->actionNewPacketWindow->setEnabled(true);
    ui->actionClosePacketWindow->setEnabled(false);
}

void
MainWindow::addMessage(rpl::Event * event)
{
    InformationWidget *infoWidget;

    messages.append(event);

    foreach(infoWidget, infoWidgets) {
        infoWidget->addMessage(event);
    }
}

void
MainWindow::clearMessages()
{
    rpl::Event * message;

    foreach(message, messages) {
        delete message;
    }
    messages.clear();

}

void
MainWindow::messageSelected(rpl::Event * event)
{
    if(!packetWidget)
        return;
    if(event == NULL)
        return;
    packetWidget->showPacket(event);
}

void
MainWindow::layoutChanged(QString layoutFile)
{
    layoutName.setText(!layoutFile.isEmpty()? layoutFile : "-");
    //Save layout in global preferences
    QSettings settings;

    settings.setValue("layout", layoutFile);
}

void
MainWindow::setDeltaColor(QTreeWidgetItem * widget, bool delta, QColor color)
{
    widget->setForeground(1, QBrush(delta ? QBrush(color) : QBrush(QColor(0, 0, 0))));
}

void
MainWindow::setTitleDeltaColor(QTreeWidgetItem * widget, bool delta, QColor color)
{
    widget->setForeground(0, QBrush(delta ? QBrush(color) : QBrush(QColor(0, 0, 0))));
}

void
MainWindow::setThreeColor(QTreeWidgetItem * widget, bool error, bool delta, QColor errorColor, QColor deltaColor)
{
    widget->setForeground(1, QBrush(error ? errorColor : (delta ? QBrush(deltaColor) : QBrush(QColor(0, 0, 0)))));
}

void
MainWindow::setTitleThreeColor(QTreeWidgetItem * widget, bool error, bool delta, QColor errorColor, QColor deltaColor)
{
    widget->setForeground(0, QBrush(error ? errorColor : (delta ? QBrush(deltaColor) : QBrush(QColor(0, 0, 0)))));
}

void
MainWindow::setTargetNodeInfo(const di_node_t * node, const di_dodag_t * dodag, const di_rpl_instance_t * rpl_instance)
{
    char ipv6string[INET6_ADDRSTRLEN];

    (void)dodag;
    (void)rpl_instance;
    if(!node) {
        clearTargetNodeInfo();
        return;
    }
    //802.15.4
    const sixlowpan_config_t *sixlowpan_config = node_get_sixlowpan_config(node);
    const sixlowpan_config_delta_t *sixlowpan_config_delta = node_get_sixlowpan_config_delta(node);

    nodeInfoTree.nodeMacAddress->setText(1, QString::number(node_get_key(node)->ref.wpan_address, 16));

    //IPv6
    setTitleDeltaColor(nodeInfoTree.ipv6Main, sixlowpan_config_delta->has_changed);
    inet_ntop(AF_INET6, (const char *)&sixlowpan_config->local_address, ipv6string, INET6_ADDRSTRLEN);
    nodeInfoTree.nodeLocalIp->setText(1, ipv6string);
    setDeltaColor(nodeInfoTree.nodeLocalIp, sixlowpan_config_delta->local_address);
    QFont nodeLocalIpFont = nodeInfoTree.nodeLocalIp->font(1);

    nodeLocalIpFont.setItalic(!sixlowpan_config->has_seen_local_address);
    nodeInfoTree.nodeLocalIp->setFont(1, nodeLocalIpFont);

    inet_ntop(AF_INET6, (const char *)&sixlowpan_config->global_address, ipv6string, INET6_ADDRSTRLEN);
    nodeInfoTree.nodeGlobalIp->setText(1, ipv6string);
    setDeltaColor(nodeInfoTree.nodeGlobalIp, sixlowpan_config_delta->global_address);
    QFont nodeGlobalIpFont = nodeInfoTree.nodeGlobalIp->font(1);

    nodeGlobalIpFont.setItalic(!sixlowpan_config->has_seen_global_address);
    nodeInfoTree.nodeGlobalIp->setFont(1, nodeGlobalIpFont);

    // RPL Instance Config
    const rpl_instance_config_t *instance_config = node_get_instance_config(node);
    const rpl_instance_config_delta_t *instance_config_delta = node_get_instance_config_delta(node);

    //const rpl_instance_config_delta_t *actual_instance_config_delta = node_get_actual_instance_config_delta(node);
    setTitleDeltaColor(nodeInfoTree.rplInstanceMain, /*actual_instance_config_delta->has_changed, */ instance_config_delta->has_changed);
    if(instance_config) {
        nodeInfoTree.rplInstanceIdMain->setText(1, QString::number(instance_config->rpl_instance_id));
        setDeltaColor(nodeInfoTree.rplInstanceIdMain, /*actual_instance_config_delta->rpl_instance_id, */ instance_config_delta->rpl_instance_id);
    } else {
        nodeInfoTree.rplInstanceIdMain->setText(1, "");
    }
    if(instance_config && instance_config->has_dodagid) {
        inet_ntop(AF_INET6, (const char *)&instance_config->dodagid, ipv6string, INET6_ADDRSTRLEN);
        nodeInfoTree.dodagId->setText(1, ipv6string);
        setDeltaColor(nodeInfoTree.dodagId, /*actual_instance_config_delta->dodagid, */ instance_config_delta->dodagid);
    } else {
        nodeInfoTree.dodagId->setText(1, "");
    }
    if(instance_config && instance_config->has_dio_config) {
        nodeInfoTree.dodagVersion->setText(1, QString::number(instance_config->version_number));
        setDeltaColor(nodeInfoTree.dodagVersion, /*actual_instance_config_delta->version_number, */ instance_config_delta->version_number);
        nodeInfoTree.dodagModeOfOperation->setText(1, QString::number(instance_config->mode_of_operation));
        setDeltaColor(nodeInfoTree.dodagModeOfOperation, /*actual_instance_config_delta->mode_of_operation, */ instance_config_delta->mode_of_operation);
    } else {
        nodeInfoTree.dodagVersion->setText(1, "");
        nodeInfoTree.dodagModeOfOperation->setText(1, "");
    }

    // RPL DODAG Config
    const rpl_dodag_config_t *dodag_config = node_get_dodag_config(node);
    const rpl_dodag_config_delta_t *dodag_config_delta = node_get_dodag_config_delta(node);
    const rpl_dodag_config_delta_t *actual_dodag_config_delta = node_get_actual_dodag_config_delta(node);

    setTitleThreeColor(nodeInfoTree.dodagConfig, actual_dodag_config_delta->has_changed, dodag_config_delta->has_changed);
    if(dodag_config) {
        nodeInfoTree.dodagConfigAuthEnabled->setText(1, (dodag_config->auth_enabled ? "Yes" : "No"));
        setThreeColor(nodeInfoTree.dodagConfigAuthEnabled, actual_dodag_config_delta->auth_enabled, dodag_config_delta->auth_enabled);
        nodeInfoTree.dodagConfigDefaultLifetime->setText(1, QString::number(dodag_config->default_lifetime));
        setThreeColor(nodeInfoTree.dodagConfigDefaultLifetime, actual_dodag_config_delta->default_lifetime, dodag_config_delta->default_lifetime);
        nodeInfoTree.dodagConfigDioIntervalMax->setText(1, QString::number(dodag_config->dio_interval_max));
        setThreeColor(nodeInfoTree.dodagConfigDioIntervalMax, actual_dodag_config_delta->dio_interval_max, dodag_config_delta->dio_interval_max);
        nodeInfoTree.dodagConfigDioIntervalMin->setText(1, QString::number(dodag_config->dio_interval_min));
        setThreeColor(nodeInfoTree.dodagConfigDioIntervalMin, actual_dodag_config_delta->dio_interval_min, dodag_config_delta->dio_interval_min);
        nodeInfoTree.dodagConfigDioRedundancyConstant->setText(1, QString::number(dodag_config->dio_redundancy_constant));
        setThreeColor(nodeInfoTree.dodagConfigDioRedundancyConstant, actual_dodag_config_delta->dio_redundancy_constant,
                      dodag_config_delta->dio_redundancy_constant);
        nodeInfoTree.dodagConfigLifetimeUnit->setText(1, QString::number(dodag_config->lifetime_unit));
        setThreeColor(nodeInfoTree.dodagConfigLifetimeUnit, actual_dodag_config_delta->lifetime_unit, dodag_config_delta->lifetime_unit);
        nodeInfoTree.dodagConfigMaxRankIncrease->setText(1, QString::number(dodag_config->max_rank_inc));
        setThreeColor(nodeInfoTree.dodagConfigMaxRankIncrease, actual_dodag_config_delta->max_rank_inc, dodag_config_delta->max_rank_inc);
        nodeInfoTree.dodagConfigMinHopRankIncrease->setText(1, QString::number(dodag_config->min_hop_rank_inc));
        setThreeColor(nodeInfoTree.dodagConfigMinHopRankIncrease, actual_dodag_config_delta->min_hop_rank_inc, dodag_config_delta->min_hop_rank_inc);
        nodeInfoTree.dodagConfigObjectiveFunction->setText(1, QString::number(dodag_config->objective_function));
        setThreeColor(nodeInfoTree.dodagConfigObjectiveFunction, actual_dodag_config_delta->objective_function, dodag_config_delta->objective_function);
        nodeInfoTree.dodagConfigPathControlSize->setText(1, QString::number(dodag_config->path_control_size));
        setThreeColor(nodeInfoTree.dodagConfigPathControlSize, actual_dodag_config_delta->path_control_size, dodag_config_delta->path_control_size);
    } else {
        nodeInfoTree.dodagConfig->setText(1, "");
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

    //RPL DODAG Prefix
    const rpl_prefix_t *prefix_info = node_get_dodag_prefix_info(node);
    const rpl_prefix_delta_t *prefix_info_delta = node_get_dodag_prefix_info_delta(node);
    const rpl_prefix_delta_t *actual_prefix_info_delta = node_get_actual_dodag_prefix_info_delta(node);

    setTitleThreeColor(nodeInfoTree.dodagPrefix, actual_prefix_info_delta->has_changed, prefix_info_delta->has_changed);
    if(prefix_info) {
        inet_ntop(AF_INET6, (const char *)&prefix_info->prefix.prefix, ipv6string, INET6_ADDRSTRLEN);
        nodeInfoTree.dodagPrefix->setText(1, QString(ipv6string) + "/" + QString::number(prefix_info->prefix.length));
        setThreeColor(nodeInfoTree.dodagPrefix, actual_prefix_info_delta->prefix, prefix_info_delta->prefix);
        nodeInfoTree.dodagPrefixOnLink->setText(1, (prefix_info->on_link ? "Yes" : "No"));
        setThreeColor(nodeInfoTree.dodagPrefixOnLink, actual_prefix_info_delta->on_link, prefix_info_delta->on_link);
        nodeInfoTree.dodagPrefixAutoconf->setText(1, (prefix_info->auto_address_config ? "Yes" : "No"));
        setThreeColor(nodeInfoTree.dodagPrefixAutoconf, actual_prefix_info_delta->auto_address_config, prefix_info_delta->auto_address_config);
        nodeInfoTree.dodagPrefixRouterAddress->setText(1, (prefix_info->router_address ? "Yes" : "No"));
        setThreeColor(nodeInfoTree.dodagPrefixRouterAddress, actual_prefix_info_delta->router_address, prefix_info_delta->router_address);
        nodeInfoTree.dodagPrefixValidLifetime->setText(1, QString::number(prefix_info->valid_lifetime));
        setThreeColor(nodeInfoTree.dodagPrefixValidLifetime, actual_prefix_info_delta->valid_lifetime, prefix_info_delta->valid_lifetime);
        nodeInfoTree.dodagPrefixPreferredLifetime->setText(1, QString::number(prefix_info->preferred_lifetime));
        setThreeColor(nodeInfoTree.dodagPrefixPreferredLifetime, actual_prefix_info_delta->preferred_lifetime, prefix_info_delta->preferred_lifetime);
    } else {
        nodeInfoTree.dodagPrefix->setText(1, "");
        nodeInfoTree.dodagPrefixOnLink->setText(1, "");
        nodeInfoTree.dodagPrefixAutoconf->setText(1, "");
        nodeInfoTree.dodagPrefixRouterAddress->setText(1, "");
        nodeInfoTree.dodagPrefixValidLifetime->setText(1, "");
        nodeInfoTree.dodagPrefixPreferredLifetime->setText(1, "");
    }

    // RPL Instance Data
    const rpl_instance_data_t *instance_data = node_get_instance_data(node);
    const rpl_instance_data_delta_t *instance_data_delta = node_get_instance_data_delta(node);

    setTitleDeltaColor(nodeInfoTree.rplDodagDataMain, instance_data_delta->has_changed);
    if(instance_data) {
        nodeInfoTree.rplDodagDataMain->setText(1, QString::number(instance_data->rpl_instance_id));
        setDeltaColor(nodeInfoTree.rplInstanceIdMain, instance_data_delta->rpl_instance_id);
    } else {
        nodeInfoTree.rplInstanceIdMain->setText(1, "");
    }
    if(instance_data && instance_data->has_dio_data) {
        nodeInfoTree.rplDodagDataMain->setText(1, QString::number(instance_data->rpl_instance_id));
        setDeltaColor(nodeInfoTree.rplInstanceIdMain, instance_data_delta->rpl_instance_id);
        nodeInfoTree.nodeGrounded->setText(1, (instance_data->grounded ? "true" : "false"));
        setDeltaColor(nodeInfoTree.nodeGrounded, instance_data_delta->grounded);
        nodeInfoTree.nodePreference->setText(1, QString::number(instance_data->preference));
        setDeltaColor(nodeInfoTree.nodePreference, instance_data_delta->preference);
        nodeInfoTree.nodeLastDtsn->setText(1, QString::number(instance_data->dtsn));
        setDeltaColor(nodeInfoTree.nodeLastDtsn, instance_data_delta->dtsn);
    } else {
        nodeInfoTree.nodeGrounded->setText(1, "");
        nodeInfoTree.nodePreference->setText(1, "");
        nodeInfoTree.nodeLastDtsn->setText(1, "");
    }
    if(instance_data && instance_data->has_dao_data) {
        nodeInfoTree.nodeLastDaoSeq->setText(1, QString::number(instance_data->latest_dao_sequence));
        setDeltaColor(nodeInfoTree.nodeLastDaoSeq, instance_data_delta->latest_dao_sequence);
    } else {
        nodeInfoTree.nodeLastDaoSeq->setText(1, "");
    }
    if(instance_data && instance_data->has_rank) {
        nodeInfoTree.nodeRank->setText(1, QString::number(instance_data->rank));
        setDeltaColor(nodeInfoTree.nodeRank, instance_data_delta->rank);
    } else {
        nodeInfoTree.nodeRank->setText(1, "");
    }

    if(instance_data && instance_data->has_metric) {
        di_metric_t metric_value = { metric_get_type("ETX"), instance_data->metric.value };
        QString metricValue = QString::number(metric_value.value);

        if(metric_value.type) {
            metricValue += QString(" (") + metric_value.type->name + ": " + QString::number(metric_get_display_value(&metric_value)) + ")";
        }
        nodeInfoTree.nodeMetric->setText(1, metricValue);
        setDeltaColor(nodeInfoTree.nodeMetric, instance_data_delta->metric);
    } else {
        nodeInfoTree.nodeMetric->setText(1, "");
    }

    //6LoWPAN statistics
    const sixlowpan_statistics_t *sixlowpan_statistics = node_get_sixlowpan_statistics(node);
    const sixlowpan_statistics_delta_t *sixlowpan_statistics_delta = node_get_sixlowpan_statistics_delta(node);

    nodeInfoTree.nodeTraffic->setText(1, QString::number(sixlowpan_statistics->packet_count));
    setDeltaColor(nodeInfoTree.nodeTraffic, sixlowpan_statistics_delta->packet_count);

    //RPL statistics
    const rpl_statistics_t *rpl_statistics = node_get_rpl_statistics(node);
    const rpl_statistics_delta_t *rpl_statistics_delta = node_get_rpl_statistics_delta(node);

    nodeInfoTree.nodeMaxDaoInterval->setText(1, QString::number(rpl_statistics->max_dao_interval) + " sec");
    setDeltaColor(nodeInfoTree.nodeMaxDaoInterval, rpl_statistics_delta->max_dao_interval);
    nodeInfoTree.nodeMaxDioInterval->setText(1, QString::number(rpl_statistics->max_dio_interval) + " sec");
    setDeltaColor(nodeInfoTree.nodeMaxDioInterval, rpl_statistics_delta->max_dio_interval);
    nodeInfoTree.nodeDis->setText(1, QString::number(rpl_statistics->dis));
    setDeltaColor(nodeInfoTree.nodeDis, rpl_statistics_delta->dis);
    nodeInfoTree.nodeDio->setText(1, QString::number(rpl_statistics->dio));
    setDeltaColor(nodeInfoTree.nodeDio, rpl_statistics_delta->dio);
    nodeInfoTree.nodeDao->setText(1, QString::number(rpl_statistics->dao));
    setDeltaColor(nodeInfoTree.nodeDao, rpl_statistics_delta->dao);

    setTitleDeltaColor(nodeInfoTree.statisticsMain, sixlowpan_statistics_delta->has_changed || rpl_statistics_delta->has_changed);

    //6LoWPAN errors
    const sixlowpan_errors_t *sixlowpan_errors = node_get_sixlowpan_errors(node);
    const sixlowpan_errors_delta_t *sixlowpan_errors_delta = node_get_sixlowpan_errors_delta(node);

    //RPL errors
    const rpl_errors_t *rpl_errors = node_get_rpl_errors(node);
    const rpl_errors_delta_t *rpl_errors_delta = node_get_rpl_errors_delta(node);

    setTitleDeltaColor(nodeInfoTree.errorsMain, rpl_errors_delta->has_changed || sixlowpan_errors_delta->has_changed, QColor(Qt::red));
    nodeInfoTree.nodeInvalidIp->setText(1, QString::number(sixlowpan_errors->invalid_ip));
    setDeltaColor(nodeInfoTree.nodeInvalidIp, sixlowpan_errors_delta->invalid_ip, QColor(Qt::red));
    nodeInfoTree.nodeInvalidPrefix->setText(1, QString::number(sixlowpan_errors->invalid_prefix));
    setDeltaColor(nodeInfoTree.nodeInvalidPrefix, sixlowpan_errors_delta->invalid_prefix, QColor(Qt::red));

    nodeInfoTree.nodeRankErrorCount->setText(1, QString::number(rpl_errors->rank_errors));
    setDeltaColor(nodeInfoTree.nodeRankErrorCount, rpl_errors_delta->rank_errors, QColor(Qt::red));
    nodeInfoTree.nodeForwardErrorCount->setText(1, QString::number(rpl_errors->forward_errors));
    setDeltaColor(nodeInfoTree.nodeForwardErrorCount, rpl_errors_delta->forward_errors, QColor(Qt::red));
    nodeInfoTree.nodeUpwardRankErrorCount->setText(1, QString::number(rpl_errors->upward_rank_errors));
    setDeltaColor(nodeInfoTree.nodeUpwardRankErrorCount, rpl_errors_delta->upward_rank_errors, QColor(Qt::red));
    nodeInfoTree.nodeDownwardRankErrorCount->setText(1, QString::number(rpl_errors->downward_rank_errors));
    setDeltaColor(nodeInfoTree.nodeDownwardRankErrorCount, rpl_errors_delta->downward_rank_errors, QColor(Qt::red));
    nodeInfoTree.nodeRouteLoopErrorCount->setText(1, QString::number(rpl_errors->route_loop_errors));
    setDeltaColor(nodeInfoTree.nodeRouteLoopErrorCount, rpl_errors_delta->route_loop_errors, QColor(Qt::red));
    nodeInfoTree.nodeIpMismatchErrorCount->setText(1, QString::number(rpl_errors->ip_mismatch_errors));
    setDeltaColor(nodeInfoTree.nodeIpMismatchErrorCount, rpl_errors_delta->ip_mismatch_errors, QColor(Qt::red));
    nodeInfoTree.nodeDodagMismatchErrorCount->setText(1, QString::number(rpl_errors->dodag_mismatch_errors));
    setDeltaColor(nodeInfoTree.nodeDodagMismatchErrorCount, rpl_errors_delta->dodag_mismatch_errors, QColor(Qt::red));
    nodeInfoTree.nodeDodagConfigMismatchErrorCount->setText(1, QString::number(rpl_errors->dodag_config_mismatch_errors));
    setDeltaColor(nodeInfoTree.nodeDodagConfigMismatchErrorCount, rpl_errors_delta->dodag_config_mismatch_errors, QColor(Qt::red));

    di_route_list_t route_table;
    di_route_el_t *route;

    route_table = node_get_routes(node);

    QList < QTreeWidgetItem * >routeChildren = nodeInfoTree.routeMain->takeChildren();
    QTreeWidgetItem *child;

    foreach(child, routeChildren)
    delete child;

    LL_FOREACH(route_table, route) {
        QStringList routeInfo;

        inet_ntop(AF_INET6, (const char *)&route->target.prefix, ipv6string, INET6_ADDRSTRLEN);
        routeInfo << QString(ipv6string) + "/" + QString::number(route->target.length) << QString::number(route->via_node, 16);
        nodeInfoTree.routeMain->addChild(new QTreeWidgetItem(routeInfo));
    }
    setTitleDeltaColor(nodeInfoTree.routeMain, node_get_routes_delta(node));
    ui->rplNodeInfoTree->resizeColumnToContents(0);
    ui->rplNodeInfoTree->resizeColumnToContents(1);
}

void
MainWindow::clearTargetNodeInfo()
{
    setTitleDeltaColor(nodeInfoTree.linkMain, false);
    nodeInfoTree.nodeMacAddress->setText(1, "");

    setTitleDeltaColor(nodeInfoTree.ipv6Main, false);
    nodeInfoTree.nodeLocalIp->setText(1, "");
    nodeInfoTree.nodeGlobalIp->setText(1, "");

    setTitleDeltaColor(nodeInfoTree.rplInstanceMain, false);
    nodeInfoTree.rplInstanceIdMain->setText(1, "");
    nodeInfoTree.dodagModeOfOperation->setText(1, "");

    setTitleDeltaColor(nodeInfoTree.dodagConfig, false);
    nodeInfoTree.dodagVersion->setText(1, "");
    nodeInfoTree.dodagId->setText(1, "");

    setTitleDeltaColor(nodeInfoTree.dodagPrefix, false);
    nodeInfoTree.dodagPrefix->setText(1, "");
    nodeInfoTree.dodagPrefixOnLink->setText(1, "");
    nodeInfoTree.dodagPrefixAutoconf->setText(1, "");
    nodeInfoTree.dodagPrefixRouterAddress->setText(1, "");
    nodeInfoTree.dodagPrefixValidLifetime->setText(1, "");
    nodeInfoTree.dodagPrefixPreferredLifetime->setText(1, "");

    setTitleDeltaColor(nodeInfoTree.dodagConfig, false);
    nodeInfoTree.dodagConfig->setText(1, "");
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

    setTitleDeltaColor(nodeInfoTree.rplDodagDataMain, false);
    nodeInfoTree.rplDodagDataMain->setText(1, "");
    nodeInfoTree.nodeMetric->setText(1, "");
    nodeInfoTree.nodeRank->setText(1, "");
    nodeInfoTree.nodeGrounded->setText(1, "");
    nodeInfoTree.nodePreference->setText(1, "");
    nodeInfoTree.nodeLastDtsn->setText(1, "");
    nodeInfoTree.nodeLastDaoSeq->setText(1, "");

    setTitleDeltaColor(nodeInfoTree.statisticsMain, false);
    nodeInfoTree.nodeTraffic->setText(1, "");
    nodeInfoTree.nodeMaxDaoInterval->setText(1, "");
    nodeInfoTree.nodeMaxDioInterval->setText(1, "");
    nodeInfoTree.nodeDis->setText(1, "");
    nodeInfoTree.nodeDio->setText(1, "");
    nodeInfoTree.nodeDao->setText(1, "");

    setTitleDeltaColor(nodeInfoTree.errorsMain, false);
    nodeInfoTree.nodeInvalidIp->setText(1, "");
    nodeInfoTree.nodeInvalidPrefix->setText(1, "");
    nodeInfoTree.nodeRankErrorCount->setText(1, "");
    nodeInfoTree.nodeForwardErrorCount->setText(1, "");
    nodeInfoTree.nodeUpwardRankErrorCount->setText(1, "");
    nodeInfoTree.nodeDownwardRankErrorCount->setText(1, "");
    nodeInfoTree.nodeRouteLoopErrorCount->setText(1, "");
    nodeInfoTree.nodeDodagMismatchErrorCount->setText(1, "");
    nodeInfoTree.nodeDodagConfigMismatchErrorCount->setText(1, "");
    nodeInfoTree.nodeIpMismatchErrorCount->setText(1, "");

    QList < QTreeWidgetItem * >routeChildren = nodeInfoTree.routeMain->takeChildren();
    QTreeWidgetItem *child;

    foreach(child, routeChildren)
    delete child;
}

void
MainWindow::onStartSniffer()
{
    onClear();
    snifferDialog->onStartSniffer();
    ui->actionStart->setEnabled(false);
    ui->actionStop->setEnabled(true);
    ui->actionOpenSnifferDialog->setEnabled(false);
    ui->versionSlider->startTime();
    captureStarted = true;
}

void
MainWindow::onStopSniffer()
{
    snifferDialog->onStopSniffer();
    ui->actionStart->setEnabled(true);
    ui->actionStop->setEnabled(false);
    ui->actionOpenSnifferDialog->setEnabled(true);
    ui->versionSlider->stopTime();
    captureStarted = false;
}

void
MainWindow::onOpenSnifferDialog()
{
    snifferDialog->exec();
    bool hasActiveSniffers = snifferDialog->activeSniffersCount() > 0;

    ui->actionStart->setEnabled(hasActiveSniffers && !captureStarted);
    ui->actionStop->setEnabled(hasActiveSniffers && captureStarted);
}

void
MainWindow::onSliderVersionChanged(int version)
{
    if(wsnManager->getDialogsLinked()) {
        emit changeWsnVersionInfo(version);
    }
}

void
MainWindow::onToggleNodeMovement(void)
{
  wsnManager->scene()->setNodeMovement(!ui->actionToggleNodeMovement->isChecked());
  QSettings settings;
  settings.setValue("nodeMovement", ui->actionToggleNodeMovement->isChecked());
}

void
MainWindow::onToggleNodeInfo(void)
{
  wsnManager->scene()->setNodeInfo(ui->actionToggleNodeInfo->isChecked());
  QSettings settings;
  settings.setValue("nodeInfo", ui->actionToggleNodeInfo->isChecked());
}

void
MainWindow::onToggleLinkDialogs(bool state)
{
  wsnManager->setLinkDialogs(state);
  QSettings settings;
  settings.setValue("linkDialogs", state);
}

void
MainWindow::onClear()
{
    rpldata_clear();
    ui->versionSlider->clearTime();
}

void
MainWindow::clearAllErrors()
{
    nodes_clear_all_errors();
}

void
MainWindow::clearDeprecatedLinks()
{
    links_clear_all_deprecated();
}

void
MainWindow::closeEvent(QCloseEvent * event)
{
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
    for(int i = 0; i < infoWidgets.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("id", infoWidgets.at(i)->objectName());
    }
    settings.endArray();

    settings.setValue("windowState", saveState());
    settings.endGroup();
    event->accept();
}
