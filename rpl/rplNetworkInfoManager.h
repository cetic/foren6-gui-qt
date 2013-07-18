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
		void nodeUpdateSelected(const di_node_t *node);

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
		Node *selectedNode;
};

}

#endif // RPLNETWORKINFOMANAGER_H
