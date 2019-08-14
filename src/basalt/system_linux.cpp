/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <sys/sysinfo.h>

#include "system.hpp"

namespace basalt {
namespace system {

uint64_t available_memory_bytes() {
    struct sysinfo info {};
    if (sysinfo(&info) != 0) {
        throw std::runtime_error(std::string("Could not get total available RAM: ") +
                                 std::strerror(errno));
    }
    return info.totalram;
}

}  // namespace system
}  // namespace basalt
