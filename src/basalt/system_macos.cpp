/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>
#include <sys/sysctl.h>
#include <sys/types.h>

#include "system.hpp"

namespace basalt {
namespace system {

uint64_t available_memory_bytes() {
    int mib[] = {CTL_HW, HW_MEMSIZE};
    uint64_t physical_memory;
    size_t length = sizeof(physical_memory);
    if (sysctl(mib, 2, &physical_memory, &length, nullptr, 0) != 0) {
        throw std::runtime_error(std::string("Could not get total available RAM: ") +
                                 std::strerror(errno));
    }
    return physical_memory;
}

}  // namespace system
}  // namespace basalt
