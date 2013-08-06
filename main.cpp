#include <QApplication>
#include "MainWindow.h"

interface_t *telos_interface;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	a.setApplicationName("RPL Diagnosis Tool");

	MainWindow m;
	m.show();
	
	return a.exec();
}
