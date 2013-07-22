#include <QApplication>
#include "RplDiagnosisTool.h"

interface_t *telos_interface;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);


	RplDiagnosisTool w;

	w.loadInterface("../capture/bin/libinterface_pcap.so");
	w.loadInterface("../capture/bin/libinterface_telos.so");

	//if(a.arguments().size() > 1)
	//	w.openSnifferTarget(a.arguments().at(1));

	
	return a.exec();
}
