#include "rplNetworkInfoManager.h"
#include <rpl_packet_parser.h>
#include "rplLink.h"
#include "rplNode.h"
#include <QGraphicsWidget>
#include <data_info/hash_container.h>
#include <data_info/link.h>

template<typename T>
T *memdup(const T *src, size_t size) {
	T *dupptr = (T*)malloc(size);

	memcpy(dupptr, src, size);

	return dupptr;
}

namespace rpl
{

NetworkInfoManager *NetworkInfoManager::thisInstance = 0;


NetworkInfoManager::NetworkInfoManager()
{
	rpl_event_callbacks_t callbacks = {
		&onNodeEvent,
		&onDodagEvent,
		&onLinkEvent,
		&onRplInstanceEvent,
		&onPacketEvent
	};

	rpl_event_set_callbacks(&callbacks);
	thisInstance = this;
	currentVersion = 0;
	selectedNode = 0;
	qRegisterMetaType<rpl::Event*>();


	_updateVersionTimer.setInterval(100);
	_updateVersionTimer.setSingleShot(false);
	QObject::connect(&_updateVersionTimer, SIGNAL(timeout()), this, SLOT(updateVersion()));
	_updateVersionTimer.start();
}

NetworkInfoManager::~NetworkInfoManager() {
}

//todo:
//move transformation from enum & data to string later in the program
//add support for packet in info model

void NetworkInfoManager::onNodeEvent(di_node_t *node, rpl_event_type_e type) {
	if(!thisInstance)
		return;
	Event *event = new Event;
	event->type = type;
	event->object = Event::EO_Node;
	event->as_node = memdup(node, node_sizeof());
	event->packed_id = rpldata_wsn_version_get_packet_count(0);
	event->version = rpldata_get_wsn_last_version();

	thisInstance->emit logMessage(event);
}

void NetworkInfoManager::onDodagEvent(di_dodag_t *dodag, rpl_event_type_e type) {
	if(!thisInstance)
		return;

	Event *event = new Event;
	event->type = type;
	event->object = Event::EO_Dodag;
	event->as_dodag = memdup(dodag, dodag_sizeof());
	event->packed_id = rpldata_wsn_version_get_packet_count(0);
	event->version = rpldata_get_wsn_last_version();

	thisInstance->emit logMessage(event);
}

void NetworkInfoManager::onRplInstanceEvent(di_rpl_instance_t *rpl_instance, rpl_event_type_e type) {
	if(!thisInstance)
		return;

	Event *event = new Event;
	event->type = type;
	event->object = Event::EO_RplInstance;
	event->as_instance = memdup(rpl_instance, rpl_instance_sizeof());
	event->packed_id = rpldata_wsn_version_get_packet_count(0);
	event->version = rpldata_get_wsn_last_version();

	thisInstance->emit logMessage(event);
}

void NetworkInfoManager::onLinkEvent(di_link_t *link, rpl_event_type_e type) {
	if(!thisInstance)
		return;

	Event *event = new Event;
	event->type = type;
	event->object = Event::EO_Link;
	event->as_link = memdup(link, rpl_instance_sizeof());
	event->packed_id = rpldata_wsn_version_get_packet_count(0);
	event->version = rpldata_get_wsn_last_version();

	thisInstance->emit logMessage(event);
}

void NetworkInfoManager::onPacketEvent(int packet_id) {
	if(!thisInstance)
		return;

	Event *event = new Event;
	event->type = RET_Created;
	event->object = Event::EO_Packet;
	event->packed_id = packet_id;
	event->version = rpldata_get_wsn_last_version();

	thisInstance->emit logMessage(event);
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

	static uint32_t last_latestversion = 0;

	if(currentVersion == 0 && rpldata_get_wsn_last_version() != last_latestversion) {
		useVersion(0);  //update only in realtime mode
		last_latestversion = rpldata_get_wsn_last_version();
	}

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
