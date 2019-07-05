/**
 * \file augment basalt Python module with bindings
 * related to NGV circuit payloads
 */

#pragma once

#include <pybind11/pybind11.h>

namespace basalt {
void register_circuit_payloads_bindings(pybind11::module& basalt);
}
