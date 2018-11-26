#ifndef BASALT_STATUS_HPP
#define BASALT_STATUS_HPP

#include <iosfwd>
#include <string>

#include <basalt/fwd.hpp>

/// /todo TCL move node_uid_t outside network_t ?

namespace basalt {
struct status_t {
    status_t(int code_, std::string message_);
    status_t() = delete;

    inline bool operator!() const noexcept { return code != 0; }
    inline explicit operator bool() const noexcept { return code == 0; }
    void raise_on_error() const;

    static const status_t& error_not_implemented();
    static status_t error_missing_node(const node_uid_t& node);
    static const status_t& ok();

    const int code;
    const std::string& message;
};

} // namespace basalt

/// \brief specialization to gently push a \a status_t to an output stream
std::ostream& operator<<(std::ostream& ostr, const basalt::status_t& status);

#endif // BASALT_STATUS_HPP
