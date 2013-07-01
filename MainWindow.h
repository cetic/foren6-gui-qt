#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "rpl/rplNetworkInfoManager.h"

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
};

#endif // MAINWINDOW_H
