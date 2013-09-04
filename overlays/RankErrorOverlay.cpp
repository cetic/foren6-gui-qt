#include "RankErrorOverlay.h"
#include "../rpl/rplNode.h"
#include "data_info/node.h"

int RankErrorOverlay::getErrorCount(rpl::Node *node) {
	di_node_t *node_data = node->getNodeData();
	return node_get_downward_error_count(node_data) + node_get_upward_error_count(node_data);
}
