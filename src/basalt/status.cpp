#include <ostream>
#include <stdexcept>
#include <sstream>

#include <basalt/network.hpp>
#include <basalt/status.hpp>

namespace basalt {

constexpr int ok_code = 0;
constexpr int missing_node_code = 2;
constexpr int not_implemented_code = 1;

static const status_t ok_{ok_code, ""};
static const status_t not_implemented_{not_implemented_code,
                                       "operation-not-implemented"};

status_t::status_t(int code_, std::string message_)
    : code(code_), message(std::move(message_)) {}

void status_t::raise_on_error() const {
    if (code != 0) {
        throw std::runtime_error(message);
    }
}

const status_t& status_t::ok() { return ok_; }

const status_t& status_t::error_not_implemented() { return not_implemented_; }

status_t status_t::error_missing_node(const node_uid_t& node) {
    std::ostringstream oss;
    oss << "Missing node " << node;
    return {missing_node_code, oss.str()};
}

} // namespace basalt

std::ostream& operator<<(std::ostream& ostr, const basalt::status_t& status) {
    return ostr << "code=" << status.code << " message=" << status.message;
}
