#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <rpl_packet_parser.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

	rpl_tool_init();
	interface = rpl_tool_get_interface("../capture/dist/Debug/GNU-Linux-x86/libcapture.so");
	if(interface == 0)
		qFatal("Could not get telos interface !");

	interface->init();

	if(QApplication::instance()->arguments().size() > 1)
		sniffer_handle = interface->open(QApplication::instance()->arguments().at(1).toUtf8().constData());
	else sniffer_handle = interface->open("/dev/ttyUSB0");

	if(sniffer_handle == 0)
		qFatal("Could not open target interface !");

	ui->setupUi(this);

	ui->rplTreeView->setScene(rplViewManager.scene());
	connect(ui->actionStart, SIGNAL(triggered()), this, SLOT(onStartSniffer()));
	connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(onStopSniffer()));
}

MainWindow::~MainWindow()
{
	interface->close(sniffer_handle);
	delete ui;
}

void MainWindow::onStartSniffer() {
	interface->start(sniffer_handle);
}

void MainWindow::onStopSniffer() {
	interface->stop(sniffer_handle);
}
