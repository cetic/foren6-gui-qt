#include "rplNetworkInfoManager.h"
#include <rpl_packet_parser.h>
#include "rplLink.h"
#include "rplNode.h"
#include <QGraphicsWidget>
#include <data_info/hash_container.h>
#include <data_info/link.h>

#include "overlays/NormalOverlay.h"

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
		&onPacketEvent,
		&onClear
	};

	rpl_event_set_callbacks(&callbacks);
	thisInstance = this;
	currentVersion = 0;
	realtimeMode = true;
	selectedNode = 0;
	qRegisterMetaType<rpl::Event*>();
	_overlay = new NormalOverlay;


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

	Node *associatedNode = reinterpret_cast<Node*>(node_get_user_data(node));
	if(associatedNode)
		Q_ASSERT(qstrcmp(associatedNode->guard, "node") == 0);

	if(type != RET_Deleted && associatedNode) {
		QMetaObject::invokeMethod(thisInstance, "updateOverlay", Qt::QueuedConnection);
	} else if(associatedNode) {
		associatedNode->setNodeData(0, -13);
	}
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

	Link *associatedLink = reinterpret_cast<Link*>(link_get_user_data(link));

	if(associatedLink)
		Q_ASSERT(qstrcmp(associatedLink->guard, "link") == 0);

	if(type != RET_Deleted && associatedLink) {
		QMetaObject::invokeMethod(thisInstance, "updateOverlay", Qt::QueuedConnection);
	} else if(associatedLink) {
		associatedLink->setLinkData(0, -14);
	}
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

void NetworkInfoManager::onClear() {
	thisInstance->emit clearMessages();
}



void NetworkInfoManager::updateOverlay() {
	QPen pen;
	QBrush brush;
	QFont font;
	QColor fontColor;
	QGraphicsItem* currentItem;
	Node *currentNode;
	Link *currentLink;

	foreach(currentItem, _scene.items()) {
		if((currentNode = dynamic_cast<Node*>(currentItem))) {
			if(_overlay->nodeCirclePen(currentNode, &pen, &brush)) {
				currentNode->setPen(pen);
				currentNode->setBrush(brush);
			}

			if(_overlay->nodeTextPen(currentNode, &font, &fontColor)) {
				currentNode->setFont(font);
				currentNode->setTextColor(fontColor);
			}
		} else if((currentLink = dynamic_cast<Link*>(currentItem))) {
			if(_overlay->linkPen(currentLink, &pen)) {
				currentLink->setPen(pen);
			}
		}
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
	updateOverlay();

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
	hash_container_ptr node_container;
	hash_container_ptr link_container;
	QHash<addr_wpan_t, Node*> presentNodes;
	QGraphicsItem *currentItem;
	Node *currentNode;

	if(version == 0) {
		version = rpldata_get_wsn_last_version();
		realtimeMode = true;
	} else {
		realtimeMode = false;
	}

	if(version && version == currentVersion)
		return;  //already at that version, nothing to do. Version 0 is a dynamic version and always change

	if(version == 0) {  //rpldata_get_wsn_last_version return 0, so there is no version
		_scene.clear();
		return;
	}

	currentVersion = version;
	node_container = rpldata_get_nodes(currentVersion);
	link_container = rpldata_get_links(version);

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
				newnode = new Node(this, node, currentVersion);
				_scene.addNode(newnode);
			}
			newnode->setNodeData(node, currentVersion);
		}
	}

	if(link_container) {
		for(hash_begin(link_container, it), hash_end(link_container, itEnd); !hash_it_equ(it, itEnd); hash_it_inc(it)) {
			di_link_t *link = *(di_link_t **)hash_it_value(it);
			Link *linkNodes;
			Node *from, *to;

			from = (Node*) node_get_user_data(*(di_node_t**)hash_value(node_container, hash_key_make(link_get_key(link)->ref.child), HVM_FailIfNonExistant, NULL));
			to =   (Node*) node_get_user_data(*(di_node_t**)hash_value(node_container, hash_key_make(link_get_key(link)->ref.parent), HVM_FailIfNonExistant, NULL));

			linkNodes = new Link(link, currentVersion, from, to);
			_scene.addLink(linkNodes);
		}
	}

	foreach(currentNode, presentNodes) {
		if(currentNode == selectedNode)
			selectedNode = 0;
		_scene.removeNode(currentNode);
		delete currentNode;
	}

	updateOverlay();

	hash_it_destroy(it);
	hash_it_destroy(itEnd);
}

void NetworkInfoManager::updateVersion() {
	const di_node_t *node_data;
	const di_dodag_t *dodag_data;
	const di_rpl_instance_t *rpl_instance_data;
	void *ptr;

	if(realtimeMode) {
		useVersion(0);
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

void NetworkInfoManager::changeOverlay(IOverlayModel* newOverlay) {
	delete _overlay;
	_overlay = newOverlay;

	updateOverlay();
}


di_node_t* NetworkInfoManager::getNode(const di_node_ref_t* node_ref) {
	void *ptr;
	int version = getVersion();

	if(version) {
		ptr = hash_value(rpldata_get_nodes(version), hash_key_make(*node_ref), HVM_FailIfNonExistant, 0);
		if(ptr)
			return *(di_node_t**)ptr;
	}

	return 0;
}

di_dodag_t* NetworkInfoManager::getDodag(const di_dodag_ref_t* dodag_ref) {
	void *ptr;
	int version = getVersion();

	if(version) {
		ptr = hash_value(rpldata_get_dodags(version), hash_key_make(*dodag_ref), HVM_FailIfNonExistant, 0);
		if(ptr)
			return *(di_dodag_t**)ptr;
	}

	return 0;
}

di_link_t* NetworkInfoManager::getLink(const di_link_ref_t* link_ref) {
	void *ptr;
	int version = getVersion();

	if(version) {
		ptr = hash_value(rpldata_get_links(version), hash_key_make(*link_ref), HVM_FailIfNonExistant, 0);
		if(ptr)
			return *(di_link_t**)ptr;
	}

	return 0;
}

di_rpl_instance_t* NetworkInfoManager::getRplInstance(const di_rpl_instance_ref_t* rpl_instance_ref) {
	void *ptr;
	int version = getVersion();

	if(version) {
		ptr = hash_value(rpldata_get_rpl_instances(version), hash_key_make(*rpl_instance_ref), HVM_FailIfNonExistant, 0);
		if(ptr)
			return *(di_rpl_instance_t**)ptr;
	}

	return 0;
}

int NetworkInfoManager::getVersion() {
	return currentVersion;
}

}
