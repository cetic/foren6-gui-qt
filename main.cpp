#include <QApplication>
#include "MainWindow.h"

interface_t *telos_interface;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    a.setApplicationName("Foren6");
    a.setApplicationVersion("1.0.0");


	MainWindow m;
	m.show();
	
	return a.exec();
}
