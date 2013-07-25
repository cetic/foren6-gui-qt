#include "RplDiagnosisTool.h"

RplDiagnosisTool::RplDiagnosisTool()
{
	rpl_tool_init();
	wsnManager = new rpl::NetworkInfoManager;
	mainWindow = new MainWindow(this);

	QObject::connect(mainWindow, SIGNAL(changeWsnVersion(int)), wsnManager, SLOT(useVersion(int)));
	QObject::connect(wsnManager, SIGNAL(nodeUpdateSelected(const di_node_t*,const di_dodag_t*,const di_rpl_instance_t*)), mainWindow, SLOT(setNodeInfoTarget(const di_node_t*,const di_dodag_t*,const di_rpl_instance_t*)));
	QObject::connect(wsnManager, SIGNAL(updateVersionCount(int)), mainWindow, SLOT(updateVersionCount(int)));
	QObject::connect(wsnManager, SIGNAL(logMessage(int,QString,QString)), mainWindow, SLOT(addMessage(int,QString,QString)));

	mainWindow->show();
}

bool RplDiagnosisTool::loadInterface(const QString& soLocation) {
	InterfaceInfo *interface;
	interface_t *currentInterface  = rpl_tool_get_interface(soLocation.toLatin1().constData());

	if(currentInterface == 0)
		return false;

	foreach(interface, loadedInterfaces) {
		if(interface->interface == currentInterface)
			return true;
	}

	interface = new InterfaceInfo;
	interface->interface = currentInterface;
	interface->openDescriptors.clear();
	loadedInterfaces.append(interface);

	return true;
}

bool RplDiagnosisTool::openSnifferTarget(const QString& target) {
	interface_t *interface;
	ifreader_t sniffer_handle;

	if(target.endsWith(".pcap"))
		interface = rpl_tool_get_interface("pcap");
	else interface = rpl_tool_get_interface("telos");

	if(interface == 0) {
		qWarning("Could not get interface !");
		return false;
	}

	interface->init();

	sniffer_handle = interface->open(target.toLatin1().constData());

	if(sniffer_handle == 0) {
		qWarning("Could not open target interface !");
		return false;
	}

	addInterfaceDescriptor(interface, sniffer_handle);

	return true;
}

void RplDiagnosisTool::onStartSniffer() {
	InterfaceInfo *interfaceInfo;
	ifreader_t handle;

	foreach(interfaceInfo, loadedInterfaces) {
		qDebug("Starting interface %s with %d descriptors", interfaceInfo->interface->interface_name, interfaceInfo->openDescriptors.size());
		foreach(handle, interfaceInfo->openDescriptors) {
			interfaceInfo->interface->start(handle);
		}
	}
}

void RplDiagnosisTool::onStopSniffer() {
	InterfaceInfo *interfaceInfo;
	ifreader_t handle;

	foreach(interfaceInfo, loadedInterfaces) {
		qDebug("Stopping interface %s with %d descriptors", interfaceInfo->interface->interface_name, interfaceInfo->openDescriptors.size());
		foreach(handle, interfaceInfo->openDescriptors) {
			interfaceInfo->interface->stop(handle);
		}
	}
}

QGraphicsScene *RplDiagnosisTool::getScene() {
	return wsnManager->scene();
}


void RplDiagnosisTool::addInterfaceDescriptor(interface_t *interface, ifreader_t desc) {
	InterfaceInfo *interfaceInfo;

	foreach(interfaceInfo, loadedInterfaces) {
		if(interfaceInfo->interface == interface && !interfaceInfo->openDescriptors.contains(desc)) {
			interfaceInfo->openDescriptors.append(desc);
			return;
		}
	}

	interfaceInfo = new InterfaceInfo;
	interfaceInfo->interface = interface;
	interfaceInfo->openDescriptors.append(desc);
	loadedInterfaces.append(interfaceInfo);
}
