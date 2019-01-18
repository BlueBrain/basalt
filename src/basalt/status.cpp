#include <ostream>
#include <sstream>
#include <stdexcept>

#include <basalt/network.hpp>
#include <basalt/status.hpp>

namespace basalt {

static const status_t ok_{status_t::ok_code, ""};
static const status_t not_implemented_{status_t::not_implemented_code, "operation-not-implemented"};

status_t::status_t(status_t::Code code_, const std::string& message_)
    : code(code_)
    , message(message_) {}

const status_t& status_t::raise_on_error() const {
    if (code != 0) {
        throw std::runtime_error(message);
    }
    return *this;
}

const status_t& status_t::ok() {
    return ok_;
}

const status_t& status_t::error_not_implemented() {
    return not_implemented_;
}

status_t status_t::error_missing_node(const node_uid_t& node) {
    std::ostringstream oss;
    oss << "Missing node " << node;
    return {missing_node_code, oss.str()};
}

status_t status_t::error_invalid_connection(const basalt::node_uid_t& node1,
                                            const basalt::node_uid_t& node2) {
    std::ostringstream oss;
    oss << "Invalid connection " << node1 << ' ' << node2;
    return {invalid_connection_code, oss.str()};
}

}  // namespace basalt

std::ostream& operator<<(std::ostream& ostr, const basalt::status_t& status) {
    return ostr << "code=" << status.code << " message=" << status.message;
}
