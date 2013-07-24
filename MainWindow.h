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

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(RplDiagnosisTool *rplDiagnosisTool);
	~MainWindow();


signals:
	void changeWsnVersion(int newVersion);

public slots:
	void setNodeInfoTarget(const di_node_t* node, const di_dodag_t* dodag, const di_rpl_instance_t* rpl_instance);
	void updateVersionCount(int versionCount);
	void changeCurrentVersion(int newVersion);
	void addMessage(int version, const QString& type, const QString& message);

protected slots:
	void onStartSniffer();
	void onStopSniffer();
	void onOpenSniffer();
	void onVersionSliderMove(int value);
	void createNewInformationWindow();
	void onInformationWindowClosed(QObject *informationWindow);

private:
	Ui::MainWindow *ui;
	RplDiagnosisTool *rplDiagnosisTool;

	QList<EventLog::Message*> messages;
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

		QTreeWidgetItem *routeMain;
	} nodeInfoTree;
};

#endif // MAINWINDOW_H
