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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QLabel>
#include "rpl/rplNetworkInfoManager.h"
#include "rpl_packet_parser.h"
#include "InformationWidget.h"
#include "PacketWidget.h"
#include "SettingsDialog.h"

namespace Ui {
    class MainWindow;
} class RplDiagnosisTool;
class QTreeWidgetItem;
class SnifferDialog;

class MainWindow:public QMainWindow {
  Q_OBJECT public:
    MainWindow(QWidget * parent = 0);
    ~MainWindow();

    static MainWindow *getInstance() {
        return thisInstance;
    }

    signals:

    void changeWsnVersion(int newVersion);
    void changeWsnVersionInfo(int newVersion);
    void reportError(QString errorMessage);

  public slots:
    void setTargetNodeInfo(const di_node_t * node, const di_dodag_t * dodag, const di_rpl_instance_t * rpl_instance);
    void clearTargetNodeInfo();
    void addMessage(rpl::Event * event);
    void clearMessages();
    void closeEvent(QCloseEvent * event);

  protected slots:
    void onStartSniffer();
    void onStopSniffer();
    void onOpenSnifferDialog();
    void onToggleNodeMovement();
    void onToggleNodeInfo();
    void onToggleLinkDialogs(bool state);
    void clearAllErrors();
    void clearDeprecatedLinks();
    void createNewInformationWindow();
    void createNewPacketWindow();
    void closePacketWindow();
    void createNewAboutWindow();
    void editSettings();
    void onInformationWindowClosed(QObject * informationWindow);
    void onPacketWindowClosed(QObject * packetWindow);
    void onClear();
    void messageSelected(rpl::Event * event);
    void onReportError(QString errorMessage);
    void layoutChanged(QString layout);
    void onSliderVersionChanged(int);

  protected:
    static void onErrorEvent(char const *errorMessage);
    void doCreateNewInformationWindow(QString name = QString());
    void setDeltaColor(QTreeWidgetItem * widget, bool delta, QColor color = QColor(Qt::blue));
    void setTitleDeltaColor(QTreeWidgetItem * widget, bool delta, QColor color = QColor(Qt::blue));
    void setThreeColor(QTreeWidgetItem * widget, bool valid, bool delta, QColor errorColor = QColor(Qt::red), QColor deltaColor = QColor(Qt::blue));
    void setTitleThreeColor(QTreeWidgetItem * widget, bool valid, bool delta, QColor errorColor = QColor(Qt::red), QColor deltaColor = QColor(Qt::blue));

  private:
    Ui::MainWindow * ui;
    rpl::NetworkInfoManager * wsnManager;
    SnifferDialog *snifferDialog;
    SettingsDialog *settingsDialog;
    bool captureStarted;
    static MainWindow *thisInstance;

    QList < rpl::Event * >messages;
    QList < InformationWidget * >infoWidgets;
    int infoWidgetId;
    PacketWidget *packetWidget;

    QLabel layoutName;

    struct NodeInfoTree {
        QTreeWidgetItem *linkMain;
        QTreeWidgetItem *nodeMacAddress;
        QTreeWidgetItem *ipv6Main;
        QTreeWidgetItem *nodeLocalIp;
        QTreeWidgetItem *nodeGlobalIp;

        QTreeWidgetItem *rplInstanceMain;
        QTreeWidgetItem *rplInstanceIdMain;

        QTreeWidgetItem *dodagId;
        QTreeWidgetItem *dodagVersion;
        QTreeWidgetItem *dodagModeOfOperation;
        QTreeWidgetItem *dodagPrefix;
        QTreeWidgetItem *dodagPrefixOnLink;
        QTreeWidgetItem *dodagPrefixAutoconf;
        QTreeWidgetItem *dodagPrefixRouterAddress;
        QTreeWidgetItem *dodagPrefixValidLifetime;
        QTreeWidgetItem *dodagPrefixPreferredLifetime;
        QTreeWidgetItem *dodagConfig;
        QTreeWidgetItem *dodagConfigAuthEnabled;
        QTreeWidgetItem *dodagConfigPathControlSize;
        QTreeWidgetItem *dodagConfigDioIntervalMin;
        QTreeWidgetItem *dodagConfigDioIntervalMax;
        QTreeWidgetItem *dodagConfigDioRedundancyConstant;
        QTreeWidgetItem *dodagConfigMaxRankIncrease;
        QTreeWidgetItem *dodagConfigMinHopRankIncrease;
        QTreeWidgetItem *dodagConfigDefaultLifetime;
        QTreeWidgetItem *dodagConfigLifetimeUnit;
        QTreeWidgetItem *dodagConfigObjectiveFunction;

        QTreeWidgetItem *rplDataMain;
        QTreeWidgetItem *rplDodagDataMain;
        QTreeWidgetItem *nodeMetric;
        QTreeWidgetItem *nodeRank;
        QTreeWidgetItem *nodeGrounded;
        QTreeWidgetItem *nodePreference;
        QTreeWidgetItem *nodeLastDtsn;
        QTreeWidgetItem *nodeLastDaoSeq;

        QTreeWidgetItem *statisticsMain;
        QTreeWidgetItem *nodeTraffic;
        QTreeWidgetItem *nodeMaxDaoInterval;
        QTreeWidgetItem *nodeMaxDioInterval;
        QTreeWidgetItem *nodeDis;
        QTreeWidgetItem *nodeDio;
        QTreeWidgetItem *nodeDao;

        QTreeWidgetItem *errorsMain;
        QTreeWidgetItem *nodeInvalidIp;
        QTreeWidgetItem *nodeInvalidPrefix;
        QTreeWidgetItem *nodeRankErrorCount;
        QTreeWidgetItem *nodeForwardErrorCount;
        QTreeWidgetItem *nodeUpwardRankErrorCount;
        QTreeWidgetItem *nodeDownwardRankErrorCount;
        QTreeWidgetItem *nodeRouteLoopErrorCount;
        QTreeWidgetItem *nodeDodagMismatchErrorCount;
        QTreeWidgetItem *nodeDodagConfigMismatchErrorCount;
        QTreeWidgetItem *nodeIpMismatchErrorCount;

        QTreeWidgetItem *routeMain;
    } nodeInfoTree;
};

#endif // MAINWINDOW_H
