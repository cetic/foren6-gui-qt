#include "rplNetworkInfoManager.h"
#include <rpl_packet_parser.h>
#include "rplLink.h"
#include "rplNode.h"
#include <QGraphicsWidget>
#include <data_info/hash_container.h>
#include <data_info/link.h>

namespace rpl
{

NetworkInfoManager *NetworkInfoManager::thisInstance = 0;


NetworkInfoManager::NetworkInfoManager()
{
	rpl_event_callbacks_t callbacks = {
	/*	.onNodeCreated = */ &onNodeEvent,
	/*	.onDodagCreated = */ &onDodagEvent,
	/*	.onLinkCreated = */  &onLinkEvent,
	/*	.onRplInstanceCreated = */ &onRplInstanceEvent
	};
	rpl_event_set_callbacks(&callbacks);
	thisInstance = this;
	currentVersion = 0;
	selectedNode = 0;
	_updateVersionTimer.setInterval(100);
	_updateVersionTimer.setSingleShot(false);
	QObject::connect(&_updateVersionTimer, SIGNAL(timeout()), this, SLOT(updateVersion()));
	_updateVersionTimer.start();
}

NetworkInfoManager::~NetworkInfoManager() {
}

QString NetworkInfoManager::eventToString(rpl_event_type_e type) {
	switch(type) {
		case RET_Created: return "created";
		case RET_Updated: return "updated";
		case RET_Deleted: return "deleted";
	}

	return "updated";
}

void NetworkInfoManager::onNodeEvent(di_node_t *node, rpl_event_type_e type) {
	if(!thisInstance)
		return;

	thisInstance->emit logMessage(rpldata_get_wsn_last_version()+1,
								  rpldata_wsn_version_get_packet_count(0),
								  QString("Node"),
								  QString("Node %1, wpan: %2")
								  .arg(eventToString(type))
								  .arg(node_get_key(node)->ref.wpan_address, 16));
}

void NetworkInfoManager::onDodagEvent(di_dodag_t *dodag, rpl_event_type_e type) {
	if(thisInstance) {
		char buffer[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &dodag_get_key(dodag)->ref.dodagid, buffer, INET6_ADDRSTRLEN);
		thisInstance->emit logMessage(rpldata_get_wsn_last_version()+1,
									  rpldata_wsn_version_get_packet_count(0),
									  QString("Dodag"),
									  QString("Dodag %1, dodagid: %2, dodag version: %3")
									  .arg(eventToString(type))
									  .arg(QString(buffer))
									  .arg(dodag_get_key(dodag)->ref.version));
	}
}

void NetworkInfoManager::onRplInstanceEvent(di_rpl_instance_t *rpl_instance, rpl_event_type_e type) {
	if(thisInstance)
		thisInstance->emit logMessage(rpldata_get_wsn_last_version()+1,
									  rpldata_wsn_version_get_packet_count(0),
									  QString("Rpl Instance"),
									  QString("Rpl Instance %1, instance: %2")
									  .arg(eventToString(type))
									  .arg(rpl_instance_get_key(rpl_instance)->ref.rpl_instance));
}

void NetworkInfoManager::onLinkEvent(di_link_t *link, rpl_event_type_e type) {
	if(thisInstance) {
		thisInstance->emit logMessage(rpldata_get_wsn_last_version()+1,
									  rpldata_wsn_version_get_packet_count(0),
									  QString("Link"),
									  QString("Link %1 from %2 to %3")
									  .arg(eventToString(type))
									  .arg(link_get_key(link)->ref.child.wpan_address, 0, 16)
									  .arg(link_get_key(link)->ref.parent.wpan_address, 0, 16));
	}
}

void NetworkInfoManager::selectNode(Node *node) {
	const di_node_t *node_data;
	const di_dodag_t *dodag_data;
	const di_rpl_instance_t *rpl_instance_data;
	const di_dodag_ref_t *dodag_ref;
	const di_rpl_instance_ref_t *rpl_instance_ref;
	void *ptr;

	if(selectedNode)
		selectedNode->setSelected(false);
	selectedNode = node;
	selectedNode->setSelected(true);

	node_data = selectedNode->getNodeData();

	dodag_ref = node_get_dodag(node_data);

	if(dodag_ref) {
		ptr = hash_value(rpldata_get_dodags(currentVersion), hash_key_make(*dodag_ref), HVM_FailIfNonExistant, 0);
		if(ptr)
			dodag_data = *(di_dodag_t**)ptr;
		else dodag_data = 0;
	} else dodag_data = 0;

	rpl_instance_ref = dodag_get_rpl_instance(dodag_data);

	if(dodag_data && rpl_instance_ref) {
		ptr = hash_value(rpldata_get_dodags(currentVersion), hash_key_make(*rpl_instance_ref), HVM_FailIfNonExistant, 0);
		if(ptr)
			rpl_instance_data = *(di_rpl_instance_t**)ptr;
		else rpl_instance_data = 0;
	} else rpl_instance_data = 0;

	emit nodeUpdateSelected(node_data, dodag_data, rpl_instance_data);
}

void NetworkInfoManager::useVersion(int version) {
	hash_iterator_ptr it, itEnd;
	hash_container_ptr node_container = rpldata_get_nodes(version);
	hash_container_ptr link_container = rpldata_get_links(version);
	QHash<addr_wpan_t, Node*> presentNodes;
	QGraphicsItem *currentItem;
	Node *currentNode;

	if(version && version == currentVersion)
		return;  //already at that version, nothing to do. Version 0 is a dynamic version and always change

	currentVersion = version;

	foreach(currentItem, _scene.items()) {
		currentNode = dynamic_cast<Node*>(currentItem);
		if(currentNode) {
			presentNodes.insert(node_get_mac64(currentNode->getNodeData()), currentNode);
			//_scene.removeItem(currentItem);
		} else if(currentItem->group() == 0)
			_scene.removeItem(currentItem);
	}

	_scene.removeAllLinks();

	it = hash_begin(NULL, NULL);
	itEnd = hash_begin(NULL, NULL);

	if(node_container) {
		for(hash_begin(node_container, it), hash_end(node_container, itEnd); !hash_it_equ(it, itEnd); hash_it_inc(it)) {
			di_node_t *node = *(di_node_t **)hash_it_value(it);
			Node *newnode;

			newnode = presentNodes.value(node_get_mac64(node), 0);
			if(newnode) {
				presentNodes.remove(node_get_mac64(node));
			} else {
				newnode = new Node(this, node, QString::number(node_get_mac64(node), 16));
				_scene.addNode(newnode);
			}
			node_set_user_data(node, newnode);
			newnode->setNodeData(node);
		}
	}

	if(link_container) {
		for(hash_begin(link_container, it), hash_end(link_container, itEnd); !hash_it_equ(it, itEnd); hash_it_inc(it)) {
			di_link_t *link = *(di_link_t **)hash_it_value(it);
			Link *linkNodes;
			Node *from, *to;

			from = (Node*) node_get_user_data(*(di_node_t**)hash_value(node_container, hash_key_make(link_get_key(link)->ref.child), HVM_FailIfNonExistant, NULL));
			to =   (Node*) node_get_user_data(*(di_node_t**)hash_value(node_container, hash_key_make(link_get_key(link)->ref.parent), HVM_FailIfNonExistant, NULL));

			linkNodes = new Link(link, from, to);
			link_set_user_data(link, linkNodes);
			_scene.addLink(linkNodes);
		}
	}

	foreach(currentNode, presentNodes) {
		if(currentNode == selectedNode)
			selectedNode = 0;
		_scene.removeNode(currentNode);
		delete currentNode;
	}

	hash_it_destroy(it);
	hash_it_destroy(itEnd);
}

void NetworkInfoManager::updateVersion() {
	const di_node_t *node_data;
	const di_dodag_t *dodag_data;
	const di_rpl_instance_t *rpl_instance_data;
	void *ptr;

	if(currentVersion == 0)
		useVersion(0);  //update only in realtime mode

	if(selectedNode) {
		const di_dodag_ref_t *dodag_ref;
		const di_rpl_instance_ref_t *rpl_instance_ref;

		node_data = selectedNode->getNodeData();

		dodag_ref = node_get_dodag(node_data);

		if(dodag_ref) {
			ptr = hash_value(rpldata_get_dodags(currentVersion), hash_key_make(*dodag_ref), HVM_FailIfNonExistant, 0);
			if(ptr)
				dodag_data = *(di_dodag_t**)ptr;
			else dodag_data = 0;
		} else dodag_data = 0;

		rpl_instance_ref = dodag_get_rpl_instance(dodag_data);

		if(dodag_data && rpl_instance_ref) {
			ptr = hash_value(rpldata_get_dodags(currentVersion), hash_key_make(*rpl_instance_ref), HVM_FailIfNonExistant, 0);
			if(ptr)
				rpl_instance_data = *(di_rpl_instance_t**)ptr;
			else rpl_instance_data = 0;
		} else rpl_instance_data = 0;

		emit nodeUpdateSelected(node_data, dodag_data, rpl_instance_data);
	}

	emit updateVersionCount(rpldata_get_wsn_last_version());
}

}
