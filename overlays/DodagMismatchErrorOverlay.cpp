#include "DodagMismatchErrorOverlay.h"
#include "../rpl/rplNode.h"
#include "data_info/node.h"

int DodagMismatchErrorOverlay::getErrorCount(rpl::Node *node) {
	di_node_t *node_data = node->getNodeData();

	return node_get_dodag_mismatch_error_count(node_data);
}
