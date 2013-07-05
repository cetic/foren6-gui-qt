#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "rpl/rplNetworkInfoManager.h"
#include "rpl_packet_parser.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
		
	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

	protected slots:
		void onStartSniffer();
		void onStopSniffer();
		
	private:
		Ui::MainWindow *ui;
		rpl::NetworkInfoManager rplViewManager;
		interface_t *interface;
		ifreader_t sniffer_handle;
};

#endif // MAINWINDOW_H
