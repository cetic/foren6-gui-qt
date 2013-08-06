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
	QObject::connect(mainWindow, SIGNAL(toggleNodeMovement()), wsnManager->scene(), SLOT(toggleNodeMovement()));

	mainWindow->show();
}

QGraphicsScene *RplDiagnosisTool::getScene() {
	return wsnManager->scene();
}
