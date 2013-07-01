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

		TreeScene *scene() { return &_scene; }

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
		void checkPendingActions();

	private:
		rpl_collector_t *_collected_data;
		TreeScene _scene;
		static NetworkInfoManager *_thisInstance;

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

		QList<Action*> _pendingActions;
		QMutex _pendingActionsMutex;
		QTimer _checkPendingActionsTimer;
};

}

#endif // RPLNETWORKINFOMANAGER_H
