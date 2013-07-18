#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "rpl/rplNetworkInfoManager.h"
#include "rpl_packet_parser.h"

namespace Ui {
class MainWindow;
}

class RplDiagnosisTool;
class QTreeWidgetItem;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(RplDiagnosisTool *rplDiagnosisTool);
	~MainWindow();

	void setVersionCount(uint32_t count);

signals:
	void changeVersion(uint32_t newVersion);

public slots:
	void setNodeInfoTarget(const di_node_t* node, const di_dodag_t* dodag, const di_rpl_instance_t* rpl_instance);

protected slots:
	void onStartSniffer();
	void onStopSniffer();
	void onSliderMove(int value);

private:
	Ui::MainWindow *ui;
	RplDiagnosisTool *rplDiagnosisTool;

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
	} nodeInfoTree;
};

#endif // MAINWINDOW_H
