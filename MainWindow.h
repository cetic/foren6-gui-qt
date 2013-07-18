#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "rpl/rplNetworkInfoManager.h"
#include "rpl_packet_parser.h"

namespace Ui {
class MainWindow;
}

class RplDiagnosisTool;

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
		void setNodeInfoTarget(const di_node_t* node);

	protected slots:
		void onStartSniffer();
		void onStopSniffer();
		void onSliderMove(int value);
		
	private:
		Ui::MainWindow *ui;
		RplDiagnosisTool *rplDiagnosisTool;
};

#endif // MAINWINDOW_H
