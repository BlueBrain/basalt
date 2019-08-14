/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#pragma once

namespace basalt {
namespace system {

/**
 * \return Total usable main memory size, in bytes
 */
uint64_t available_memory_bytes();

}  // namespace system
}  // namespace basalt
