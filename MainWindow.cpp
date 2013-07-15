#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <rpl_packet_parser.h>
#include "rpl/rplNetworkInfoManager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	char target[128] = "/dev/ttyUSB0";
	QStringList arguments = QApplication::instance()->arguments();
	rpl_tool_init();

	qDebug("%d arguments:", arguments.size());

	for(int i=0; i < arguments.size(); i++) {
		qDebug("Argument: %s", arguments.at(i).toUtf8().constData());
	}

	if(arguments.size() > 1)
		qstrcpy(target, arguments.at(1).toUtf8().constData());

	if(QString(target).endsWith(".pcap"))
		interface = rpl_tool_get_interface("../capture/bin/libinterface_pcap.so");
	else interface = rpl_tool_get_interface("../capture/bin/libinterface_telos.so");

	if(interface == 0)
		qFatal("Could not get telos interface !");

	interface->init();

	sniffer_handle = interface->open(target);

	if(sniffer_handle == 0)
		qFatal("Could not open target interface !");

	ui->setupUi(this);
	ui->infoSplitter->setStretchFactor(0, 0);
	ui->infoSplitter->setStretchFactor(1, 1);
	ui->logSplitter->setStretchFactor(0, 3);
	ui->logSplitter->setStretchFactor(1, 1);

	ui->rplTreeView->setScene(rplViewManager.scene());
	connect(ui->actionStart, SIGNAL(triggered()), this, SLOT(onStartSniffer()));
	connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(onStopSniffer()));
	connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderMove(int)));
}

MainWindow::~MainWindow()
{
	interface->close(sniffer_handle);
	rpl_tool_cleanup();
	delete ui;
}

void MainWindow::onStartSniffer() {
	interface->start(sniffer_handle);
	ui->logEdit->append(QString("Currently %1 versions").arg(rpldata_get_node_last_version()));
}

void MainWindow::onStopSniffer() {
	interface->stop(sniffer_handle);
}

void MainWindow::onSliderMove(int value) {
	uint32_t version = value*rpldata_get_node_last_version()/ui->horizontalSlider->maximum();

	if(value == ui->horizontalSlider->maximum())
		version = 0;  //realtime mode
	else if(version == 0)
		version = 1; //When the slide is at left most, use the first version which is version 1 (version 0 is the realtime version)

	ui->logEdit->append(QString("Using version %1").arg(version));

	rplViewManager.useVersion(version);
}
