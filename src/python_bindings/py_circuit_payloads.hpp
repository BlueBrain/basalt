/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
/**
 * \file augment basalt Python module with bindings
 * related to NGV circuit payloads
 */

#pragma once

#include <pybind11/pybind11.h>

namespace basalt {
void register_circuit_payloads_bindings(pybind11::module& basalt);
}
