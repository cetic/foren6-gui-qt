#include "rplNetworkInfoManager.h"
#include <rpl_packet_parser.h>
#include "rplLink.h"
#include "rplNode.h"
#include <QGraphicsWidget>

namespace rpl
{

NetworkInfoManager *NetworkInfoManager::_thisInstance = 0;

NetworkInfoManager::NetworkInfoManager()
{
	rpl_event_callbacks_t callbacks = {
		&onNodeCreated,
		&onNodeDeleted,
		&onNodeUpdated,
		0,
		0,
		0,
		&onLinkCreated,
		&onLinkDeleted,
		0,
		0,
		0,
		0
	};
	_collected_data = rpl_tool_get_collected_data();
	_thisInstance = this;
	rpl_tool_set_callbacks(&callbacks);
	_checkPendingActionsTimer.setInterval(100);
	_checkPendingActionsTimer.setSingleShot(false);
	QObject::connect(&_checkPendingActionsTimer, SIGNAL(timeout()), this, SLOT(checkPendingActions()));
	_checkPendingActionsTimer.start();
}

void NetworkInfoManager::onNodeCreated(di_node_t *node) {
	QMutexLocker lockMutex(&_thisInstance->_pendingActionsMutex);
	Action *action = new Action;
	action->event = Action::AE_Created;
	action->type = Action::AT_Node;
	action->ptr = node;
	qDebug("Node created: %p, %llX", node, node->wpan_address);
	_thisInstance->_pendingActions.append(action);
}

void NetworkInfoManager::onNodeUpdated(di_node_t *node) {
	QMutexLocker lockMutex(&_thisInstance->_pendingActionsMutex);
	Action *action = new Action;
	action->event = Action::AE_Updated;
	action->type = Action::AT_Node;
	action->ptr = node;
	_thisInstance->_pendingActions.append(action);
}

void NetworkInfoManager::onNodeDeleted(di_node_t *node) {
	QMutexLocker lockMutex(&_thisInstance->_pendingActionsMutex);
	Action *action = new Action;
	action->event = Action::AE_Deleted;
	action->type = Action::AT_Node;
	action->ptr = node->user_data;
	qDebug("Node deleted: %p, %llX", node, node->wpan_address);
	_thisInstance->_pendingActions.append(action);
}

void NetworkInfoManager::onDodagCreated(di_dodag_t *dodag) {
	QMutexLocker lockMutex(&_thisInstance->_pendingActionsMutex);
	Action *action = new Action;
	action->event = Action::AE_Created;
	action->type = Action::AT_Dodag;
	action->ptr = dodag;
	_thisInstance->_pendingActions.append(action);
}

void NetworkInfoManager::onDodagUpdated(di_dodag_t *dodag) {
	QMutexLocker lockMutex(&_thisInstance->_pendingActionsMutex);
	Action *action = new Action;
	action->event = Action::AE_Updated;
	action->type = Action::AT_Dodag;
	action->ptr = dodag;
	_thisInstance->_pendingActions.append(action);
}

void NetworkInfoManager::onRplInstanceCreated(di_rpl_instance_t *rplInstance) {
	QMutexLocker lockMutex(&_thisInstance->_pendingActionsMutex);
	Action *action = new Action;
	action->event = Action::AE_Created;
	action->type = Action::AT_RplInstance;
	action->ptr = rplInstance;
	_thisInstance->_pendingActions.append(action);
}

void NetworkInfoManager::onRplInstanceUpdated(di_rpl_instance_t *rplInstance) {
	QMutexLocker lockMutex(&_thisInstance->_pendingActionsMutex);
	Action *action = new Action;
	action->event = Action::AE_Updated;
	action->type = Action::AT_RplInstance;
	action->ptr = rplInstance;
	_thisInstance->_pendingActions.append(action);
}

void NetworkInfoManager::onLinkCreated(di_link_t *link) {
	QMutexLocker lockMutex(&_thisInstance->_pendingActionsMutex);
	Action *action = new Action;
	action->event = Action::AE_Created;
	action->type = Action::AT_Link;
	action->ptr = link;
	qDebug("Link created: %p, %llX -> %llX", link, link->key.child->wpan_address, link->key.parent->wpan_address);
	_thisInstance->_pendingActions.append(action);
}

void NetworkInfoManager::onLinkDeleted(di_link_t *link) {
	QMutexLocker lockMutex(&_thisInstance->_pendingActionsMutex);
	Action *action = new Action;
	action->event = Action::AE_Deleted;
	action->type = Action::AT_Link;
	action->ptr = link->user_data;
	qDebug("Link deleted: %p, %llX -> %llX", link, link->key.child->wpan_address, link->key.parent->wpan_address);
	_thisInstance->_pendingActions.append(action);
	if((unsigned int)link->user_data < 1000 && link->user_data) {
		qDebug("WTF");
	}
}


void NetworkInfoManager::checkPendingActions() {
	QMutexLocker lockMutex(&_pendingActionsMutex);
	Action* action;

	foreach(action, _pendingActions) {
		switch(action->type) {
			case Action::AT_Link: {
				if(action->event == Action::AE_Created) {
					di_link_t *link = static_cast<di_link_t*>(action->ptr);
					Link *linkNodes;

					linkNodes = new Link(link);
					link->user_data = linkNodes;
					_thisInstance->_scene.addLink(linkNodes);
				} else if(action->event == Action::AE_Deleted) {
					Link *link = static_cast<Link*>(action->ptr);
					_thisInstance->_scene.removeLink(link);
					delete link;
				}
				break;
			}
			case Action::AT_Node: {
				di_node_t *node = static_cast<di_node_t*>(action->ptr);
				if(action->event == Action::AE_Created) {
					Node *newnode;

					newnode = new Node(node, QString::number(node->wpan_address, 16));
					node->user_data = newnode;
					_thisInstance->_scene.addNode(newnode);
				} else if(action->event == Action::AE_Deleted) {
					Node *node = static_cast<Node*>(action->ptr);
					_thisInstance->_scene.removeNode(node);
					delete node;
				} else if(action->event == Action::AE_Updated) {
					static_cast<Node*>(node->user_data)->onNodeChanged();
				}
				break;
			}
		}
		delete action;
	}
	_pendingActions.clear();
}

}
