#ifndef RPLDIAGNOSISTOOL_H
#define RPLDIAGNOSISTOOL_H

#include "rpl/rplNetworkInfoManager.h"
#include "MainWindow.h"
#include "rpl_packet_parser.h"

class RplDiagnosisTool
{
	public:
		RplDiagnosisTool();

		bool loadInterface(const QString& soLocation);
		bool openSnifferTarget(const QString& target);

		void onStartSniffer();
		void onStopSniffer();

		QGraphicsScene *getScene();

	protected:
		void addInterfaceDescriptor(interface_t *interface, ifreader_t desc);


	private:
		struct InterfaceInfo {
			interface_t *interface;
			QList<ifreader_t> openDescriptors;
		};

		QList<InterfaceInfo*> loadedInterfaces;

		rpl::NetworkInfoManager *wsnManager;
		MainWindow *mainWindow;
};

#endif // RPLDIAGNOSISTOOL_H
