#ifndef BASALT_STATUS_HPP
#define BASALT_STATUS_HPP

#include <iosfwd>
#include <string>

#include <basalt/fwd.hpp>

namespace basalt {
struct status_t {
    enum Code {
        ok_code = 0,
        not_implemented_code = -1,
        missing_node_code = -2,
        invalid_connection_code = -3
    };

    status_t(Code code_, const std::string& message_);
    status_t() = delete;

    inline bool operator!() const noexcept { return code != 0; }
    inline explicit operator bool() const noexcept { return code == 0; }
    const status_t& raise_on_error() const;

    static const status_t& error_not_implemented();
    static status_t error_missing_node(const node_uid_t& node);
    static status_t error_invalid_connection(const node_uid_t& node1,
                                             const node_uid_t& node2);
    static const status_t& ok();

    const Code code;
    const std::string message;
};

} // namespace basalt

/// \brief specialization to gently push a \a status_t to an output stream
std::ostream& operator<<(std::ostream& ostr, const basalt::status_t& status);

#endif // BASALT_STATUS_HPP
