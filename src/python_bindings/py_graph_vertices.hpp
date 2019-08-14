/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
/**
 * \file augment basalt Python module with bindings
 * for graph vertices wrapper class
 */

#pragma once

#include <pybind11/pybind11.h>

namespace basalt {

void register_graph_vertices(pybind11::module& m);

}
