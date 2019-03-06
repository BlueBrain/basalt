/**
 * \file augment basalt Python module with bindings
 * for graph edges wrapper class
 */

#pragma once

#include <pybind11/pybind11.h>

namespace basalt {

void register_graph_edges(pybind11::module& m);

}
