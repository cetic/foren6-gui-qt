#include "RouteLoopErrorOverlay.h"
#include "../rpl/rplNode.h"
#include "data_info/node.h"

int RouteLoopErrorOverlay::getErrorCount(rpl::Node *node) {
	di_node_t *node_data = node->getNodeData();
    const rpl_errors_t *rpl_errors = node_get_rpl_errors(node_data);

    return rpl_errors->route_loop_errors;
}
