#include "NormalOverlay.h"
#include "../rpl/rplNode.h"
#include "data_info/node.h"

int NormalOverlay::getErrorCount(rpl::Node *node) {
    di_node_t *node_data = node->getNodeData();
    return node_get_downward_error_count(node_data) + node_get_upward_error_count(node_data) +
       node_get_route_error_count(node_data) +  node_get_ip_mismatch_error_count(node_data) +
       /*node_get_dodag_version_decrease_error_count(node_data) +*/ node_get_dodag_mismatch_error_count(node_data);
}
