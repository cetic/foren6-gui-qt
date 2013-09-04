#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "rpl/rplNetworkInfoManager.h"
#include "rpl_packet_parser.h"
#include "InformationWidget.h"

namespace Ui {
class MainWindow;
}

class RplDiagnosisTool;
class QTreeWidgetItem;
class SnifferDialog;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();


signals:
	void changeWsnVersion(int newVersion);
	void toggleNodeMovement();

public slots:
	void setNodeInfoTarget(const di_node_t* node, const di_dodag_t* dodag, const di_rpl_instance_t* rpl_instance);
	void addMessage(rpl::Event *event);
	void clearMessages();

protected slots:
	void onStartSniffer();
	void onStopSniffer();
	void onOpenSnifferDialog();
	void createNewInformationWindow();
	void onInformationWindowClosed(QObject *informationWindow);
	void onClear();

private:
	Ui::MainWindow *ui;
	rpl::NetworkInfoManager *wsnManager;
	SnifferDialog *snifferDialog;

	QList<rpl::Event*> messages;
	QList<InformationWidget*> infoWidgets;

	struct NodeInfoTree {
		QTreeWidgetItem *rplInstanceMain;
		QTreeWidgetItem *rplInstanceId;
		QTreeWidgetItem *rplInstanceModeOfOperation;

		QTreeWidgetItem *dodagMain;
		QTreeWidgetItem *dodagVersion;
		QTreeWidgetItem *dodagId;
		QTreeWidgetItem *dodagPrefix;
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

		QTreeWidgetItem *nodeMain;
		QTreeWidgetItem *nodeMetric;
		QTreeWidgetItem *nodeRank;
		QTreeWidgetItem *nodeMacAddress;
		QTreeWidgetItem *nodeLocalIp;
		QTreeWidgetItem *nodeGlobalIp;
		QTreeWidgetItem *nodeGrounded;
		QTreeWidgetItem *nodeTraffic;
		QTreeWidgetItem *nodeMaxDaoInterval;
		QTreeWidgetItem *nodeMaxDioInterval;
		QTreeWidgetItem *nodeLastDtsn;
		QTreeWidgetItem *nodeLastDaoSeq;
		QTreeWidgetItem *nodeUpwardRankErrorCount;
		QTreeWidgetItem *nodeDownwardRankErrorCount;
		QTreeWidgetItem *nodeRouteLoopErrorCount;
		QTreeWidgetItem *nodeDodagMismatchErrorCount;
		QTreeWidgetItem *nodeIpMismatchErrorCount;

		QTreeWidgetItem *routeMain;
	} nodeInfoTree;
};

#endif // MAINWINDOW_H
