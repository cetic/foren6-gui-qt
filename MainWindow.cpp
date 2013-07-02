#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <rpl_packet_parser.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->rplTreeView->setScene(rplViewManager.scene());
	connect(ui->actionStart, SIGNAL(triggered()), this, SLOT(onStartSniffer()));
	connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(onStopSniffer()));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::onStartSniffer() {
	rpl_tool_start();
}

void MainWindow::onStopSniffer() {
	rpl_tool_stop();
}
