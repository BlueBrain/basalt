#include <ostream>
#include <stdexcept>

#include <basalt/status.hpp>

namespace basalt {

status_t::status_t(int code_, std::string message_)
    : code(code_), message(std::move(message_)) {}

static const status_t not_implemented_{1, "operation-not-implemented"};

void status_t::raise_on_error() const {
    if (code != 0) {
        throw std::runtime_error(message);
    }
}

const status_t& status_t::not_implemented() { return not_implemented_; }

} // namespace basalt

std::ostream& operator<<(std::ostream& ostr, const basalt::status_t& status) {
    return ostr << "code=" << status.code << " message=" << status.message;
}
