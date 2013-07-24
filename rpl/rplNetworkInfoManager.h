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

		void selectNode(Node *node);

	public slots:
		void useVersion(uint32_t version);

	signals:
		void nodeUpdateSelected(const di_node_t *node, const di_dodag_t* dodag, const di_rpl_instance_t* rpl_instance);
		void updateVersionCount(uint32_t versionCount);
		void logMessage(int version, const QString& type, const QString& message);

	protected slots:
		void updateVersion();

		static void onNodeCreated(di_node_t *node);
		static void onDodagCreated(di_dodag_t *dodag);

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
		Node *selectedNode;
		static NetworkInfoManager *thisInstance;
};

}

#endif // RPLNETWORKINFOMANAGER_H
