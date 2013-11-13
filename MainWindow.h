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
    } signals:void changeWsnVersion(int newVersion);
    void changeWsnVersionInfo(int newVersion);
    void toggleNodeMovement();
    void reportError(QString errorMessage);

    public slots:void setTargetNodeInfo(const di_node_t * node, const di_dodag_t * dodag, const di_rpl_instance_t * rpl_instance);
    void clearTargetNodeInfo();
    void addMessage(rpl::Event * event);
    void clearMessages();
    void closeEvent(QCloseEvent * event);

    protected slots:void onStartSniffer();
    void onStopSniffer();
    void onOpenSnifferDialog();
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
