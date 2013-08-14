#ifndef RPLNETWORKINFOMANAGER_H
#define RPLNETWORKINFOMANAGER_H

#include <QHash>
#include <rpl_packet_parser.h>
#include "rplTreeScene.h"
#include <QMutex>
#include <QObject>

namespace rpl
{

struct Event {
	rpl_event_type_e type;
	enum {
		EO_Node,
		EO_Dodag,
		EO_RplInstance,
		EO_Link,
		EO_Packet
	} object;
	union {
		di_node_t *as_node;
		di_dodag_t *as_dodag;
		di_rpl_instance_t *as_instance;
		di_link_t *as_link;
	};
	int version;
	int packed_id;
};

class NetworkInfoManager : public QObject
{
	Q_OBJECT
public:

public:
	NetworkInfoManager();
	~NetworkInfoManager();

	TreeScene *scene() { return &_scene; }

	void selectNode(Node *node);
	IOverlayModel* overlay() { return _overlay; }
	void changeOverlay(IOverlayModel* newOverlay);

public slots:
	void useVersion(int version);

signals:
	void nodeUpdateSelected(const di_node_t *node, const di_dodag_t* dodag, const di_rpl_instance_t* rpl_instance);
	void updateVersionCount(int versionCount);
	void logMessage(rpl::Event *action);

protected slots:
	void updateVersion();
	void updateNodeOverlay(void *node) { updateNodeOverlay(reinterpret_cast<Node*>(node)); }
	void updateLinkOverlay(void *link) { updateLinkOverlay(reinterpret_cast<Link*>(link)); }

protected:
	static void onNodeEvent(di_node_t *node, rpl_event_type_e type);
	static void onDodagEvent(di_dodag_t *dodag, rpl_event_type_e type);
	static void onRplInstanceEvent(di_rpl_instance_t *rpl_instance, rpl_event_type_e type);
	static void onLinkEvent(di_link_t *link, rpl_event_type_e type);
	static void onPacketEvent(int packet_id);

	void updateNodeOverlay(Node *node);
	void updateLinkOverlay(Link *link);

private:
	TreeScene _scene;
	IOverlayModel* _overlay;

	QTimer _updateVersionTimer;
	int currentVersion;
	Node *selectedNode;
	static NetworkInfoManager *thisInstance;
};

}

Q_DECLARE_METATYPE(rpl::Event*)

#endif // RPLNETWORKINFOMANAGER_H
