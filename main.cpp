#include <QApplication>
#include "MainWindow.h"

interface_t *telos_interface;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);


	MainWindow w;
	w.show();

	
	return a.exec();
}
