/**
 * \file augment basalt Python module with bindings
 * for graph vertices wrapper class
 */

#pragma once

#include <pybind11/pybind11.h>

namespace basalt {

void register_graph_vertices(pybind11::module& m);

}
