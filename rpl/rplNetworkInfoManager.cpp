#include "rplNetworkInfoManager.h"
#include <rpl_packet_parser.h>
#include "rplLink.h"
#include "rplNode.h"
#include <QGraphicsWidget>
#include <data_info/hash_container.h>

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

NetworkInfoManager::~NetworkInfoManager() {
	_checkPendingActionsTimer.stop();
	QMutexLocker lockMutex(&_thisInstance->_pendingActionsMutex);
	_collected_data = 0;
	_thisInstance = 0;

}

void NetworkInfoManager::onNodeCreated(di_node_t *node) {
	QMutexLocker lockMutex(&_thisInstance->_pendingActionsMutex);
	Action *action = new Action;
	action->event = Action::AE_Created;
	action->type = Action::AT_Node;
	action->ptr = node;
	qDebug("Node created: %p, %llX", node, node_get_mac64(node));
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
	action->ptr = node_get_user_data(node);
	qDebug("Node deleted: %p, %llX", node, node_get_mac64(node));
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
	qDebug("Link created: %p, %llX -> %llX", link, link->key.ref.child.wpan_address, link->key.ref.parent.wpan_address);
	_thisInstance->_pendingActions.append(action);
}

void NetworkInfoManager::onLinkDeleted(di_link_t *link) {
	QMutexLocker lockMutex(&_thisInstance->_pendingActionsMutex);
	Action *action = new Action;
	action->event = Action::AE_Deleted;
	action->type = Action::AT_Link;
	action->ptr = link->user_data;
	qDebug("Link deleted: %p, %llX -> %llX", link, link->key.ref.child.wpan_address, link->key.ref.parent.wpan_address);
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
					Node *from, *to;
					di_node_key_t node_key;

					node_key = (di_node_key_t){link->key.ref.child, 0};
					from = (Node*) node_get_user_data((di_node_t*)hash_value(_collected_data->nodes, hash_key_make(node_key), HVM_FailIfNonExistant, NULL));


					node_key = (di_node_key_t){link->key.ref.parent, 0};
					to =   (Node*) node_get_user_data((di_node_t*)hash_value(_collected_data->nodes, hash_key_make(node_key), HVM_FailIfNonExistant, NULL));

					linkNodes = new Link(link, from, to);
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

					newnode = new Node(node, QString::number(node_get_mac64(node), 16));
					node_set_user_data(node, newnode);
					_thisInstance->_scene.addNode(newnode);
				} else if(action->event == Action::AE_Deleted) {
					Node *node = static_cast<Node*>(action->ptr);
					_thisInstance->_scene.removeNode(node);
					delete node;
				} else if(action->event == Action::AE_Updated) {
					static_cast<Node*>(node_get_user_data(node))->onNodeChanged();
				}
				break;
			}
		}
		delete action;
	}
	_pendingActions.clear();
}

}
