#include "RouteLoopErrorOverlay.h"
#include "../rpl/rplNode.h"
#include "data_info/node.h"

int RouteLoopErrorOverlay::getErrorCount(rpl::Node *node) {
	di_node_t *node_data = node->getNodeData();
	return node_get_route_error_count(node_data);
}
