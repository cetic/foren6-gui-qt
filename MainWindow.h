#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QLabel>
#include "rpl/rplNetworkInfoManager.h"
#include "rpl_packet_parser.h"
#include "InformationWidget.h"
#include "PacketWidget.h"

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

	static MainWindow * getInstance() { return thisInstance; }

signals:
	void changeWsnVersion(int newVersion);
	void toggleNodeMovement();
    void reportError(QString errorMessage);

public slots:
	void setTargetNodeInfo(const di_node_t* node, const di_dodag_t* dodag, const di_rpl_instance_t* rpl_instance);
	void clearTargetNodeInfo();
	void addMessage(rpl::Event *event);
	void clearMessages();
    void closeEvent(QCloseEvent *event);

protected slots:
	void onStartSniffer();
	void onStopSniffer();
	void onOpenSnifferDialog();
	void createNewInformationWindow();
    void createNewPacketWindow();
    void createNewAboutWindow();
	void onInformationWindowClosed(QObject *informationWindow);
    void onPacketWindowClosed(QObject *packetWindow);
	void onClear();
	void messageSelected(rpl::Event *  event);
    void onReportError(QString errorMessage);
    void layoutChanged(QString layout);

protected:
	static void onErrorEvent(char const * errorMessage);
	void doCreateNewInformationWindow(QString name = QString());
	void setDeltaColor(QTreeWidgetItem * widget, bool delta, QColor color = QColor(Qt::blue));
    void setTitleDeltaColor(QTreeWidgetItem * widget, bool delta, QColor color = QColor(Qt::blue));
    void setThreeColor(QTreeWidgetItem * widget, bool valid, bool delta, QColor errorColor = QColor(Qt::red), QColor deltaColor = QColor(Qt::blue));
    void setTitleThreeColor(QTreeWidgetItem * widget, bool valid, bool delta, QColor errorColor = QColor(Qt::red), QColor deltaColor = QColor(Qt::blue));

private:
	Ui::MainWindow *ui;
	rpl::NetworkInfoManager *wsnManager;
	SnifferDialog *snifferDialog;
	bool captureStarted;
	static MainWindow *  thisInstance;

	QList<rpl::Event*> messages;
	QList<InformationWidget*> infoWidgets;
	int infoWidgetId;
    PacketWidget* packetWidget;

	QLabel layoutName;

	struct NodeInfoTree {
		QTreeWidgetItem *rplInstanceMain;
		QTreeWidgetItem *rplInstanceId;
		QTreeWidgetItem *rplInstanceModeOfOperation;

		QTreeWidgetItem *dodagMain;
		QTreeWidgetItem *dodagVersion;
		QTreeWidgetItem *dodagId;
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
