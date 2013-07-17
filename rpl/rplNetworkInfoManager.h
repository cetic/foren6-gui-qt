#ifndef RPLNETWORKINFOMANAGER_H
#define RPLNETWORKINFOMANAGER_H

#include <QHash>
#include <rpl_packet_parser.h>
#include "rplTreeScene.h"
#include <QMutex>
#include <QObject>

namespace rpl
{

class NetworkInfoManager : public QObject
{
		Q_OBJECT
	public:
		NetworkInfoManager();
		~NetworkInfoManager();

		TreeScene *scene() { return &_scene; }
		void useVersion(uint32_t version);

	protected:
		static void onNodeCreated(di_node_t *node);
		static void onNodeUpdated(di_node_t *node);
		static void onNodeDeleted(di_node_t *node);

		static void onDodagCreated(di_dodag_t *dodag);
		static void onDodagUpdated(di_dodag_t *dodag);

		static void onRplInstanceCreated(di_rpl_instance_t *rplInstance);
		static void onRplInstanceUpdated(di_rpl_instance_t *rplInstance);

		static void onLinkCreated(di_link_t *link);
		static void onLinkDeleted(di_link_t *link);

	protected slots:
		void updateVersion();

	private:
		TreeScene _scene;

		struct Action {
			enum {
				AE_Created,
				AE_Deleted,
				AE_Updated
			} event;
			enum {
				AT_Link,
				AT_Node,
				AT_RplInstance,
				AT_Dodag
			} type;
			void *ptr;
		};

		QTimer _updateVersionTimer;
		uint32_t currentVersion;
};

}

#endif // RPLNETWORKINFOMANAGER_H
