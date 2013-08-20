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

	int getVersion();
	di_node_t* getNode(const di_node_ref_t* node_ref);
	di_dodag_t* getDodag(const di_dodag_ref_t* dodag_ref);
	di_link_t* getLink(const di_link_ref_t* link_ref);
	di_rpl_instance_t* getRplInstance(const di_rpl_instance_ref_t* rpl_instance_ref);

	static NetworkInfoManager* getInstance() { return thisInstance; }

public slots:
	void useVersion(int version);
	void updateOverlay();

signals:
	void nodeUpdateSelected(const di_node_t *node, const di_dodag_t* dodag, const di_rpl_instance_t* rpl_instance);
	void updateVersionCount(int versionCount);
	void logMessage(rpl::Event *action);

protected slots:
	void updateVersion();

protected:
	static void onNodeEvent(di_node_t *node, rpl_event_type_e type);
	static void onDodagEvent(di_dodag_t *dodag, rpl_event_type_e type);
	static void onRplInstanceEvent(di_rpl_instance_t *rpl_instance, rpl_event_type_e type);
	static void onLinkEvent(di_link_t *link, rpl_event_type_e type);
	static void onPacketEvent(int packet_id);

private:
	TreeScene _scene;
	IOverlayModel* _overlay;

	QTimer _updateVersionTimer;
	int currentVersion;
	bool realtimeMode;
	Node *selectedNode;
	static NetworkInfoManager *thisInstance;
};

}

Q_DECLARE_METATYPE(rpl::Event*)

#endif // RPLNETWORKINFOMANAGER_H
