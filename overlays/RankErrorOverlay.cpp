/*
 * This file is part of Foren6, a 6LoWPAN Diagnosis Tool
 * Copyright (C) 2013, CETIC
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * \file
 *         Rank Errors Overlay
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include "RankErrorOverlay.h"
#include "../rpl/rplNode.h"
#include "data_info/node.h"

int
RankErrorOverlay::getErrorCount(rpl::Node * node)
{
    di_node_t *node_data = node->getNodeData();
    const rpl_errors_t *rpl_errors = node_get_rpl_errors(node_data);

    return rpl_errors->downward_rank_errors + rpl_errors->upward_rank_errors;
}
