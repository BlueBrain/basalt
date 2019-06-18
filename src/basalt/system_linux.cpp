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
