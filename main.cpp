#include <QApplication>
#include "MainWindow.h"
#include "rpl_packet_parser.h"
#include "rpl/rplTreeScene.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	if(a.arguments().size() > 1)
		rpl_tool_init("../capture/dist/Debug/GNU-Linux-x86/libcapture.so", a.arguments().at(1).toUtf8().constData());
	else rpl_tool_init("../capture/dist/Debug/GNU-Linux-x86/libcapture.so", "/dev/ttyUSB0");


	MainWindow w;
	w.show();

	
	return a.exec();
}
