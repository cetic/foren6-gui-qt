#include "IPMismatchErrorOverlay.h"
#include "../rpl/rplNode.h"
#include "data_info/node.h"

int IPMismatchErrorOverlay::getErrorCount(rpl::Node *node) {
	di_node_t *node_data = node->getNodeData();

	return node_get_ip_mismatch_error_count(node_data);
}
