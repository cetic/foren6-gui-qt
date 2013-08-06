#include <QApplication>
#include "RplDiagnosisTool.h"

interface_t *telos_interface;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	a.setApplicationName("RPL Diagnosis Tool");


	RplDiagnosisTool w;

	//if(a.arguments().size() > 1)
	//	w.openSnifferTarget(a.arguments().at(1));

	
	return a.exec();
}
