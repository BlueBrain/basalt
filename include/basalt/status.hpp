#ifndef BASALT_STATUS_HPP
#define BASALT_STATUS_HPP

#include <iosfwd>
#include <string>

namespace basalt {
struct status_t {
    status_t(int code_, std::string message_);
    status_t() = delete;

    inline bool operator!() const noexcept { return code != 0; }
    inline explicit operator bool() const noexcept { return code == 0; }
    void raise_on_error() const;
    static const status_t& not_implemented();

    const int code;
    const std::string& message;
};

} // namespace basalt

/// \brief specialization to gently push a \a status_t to an output stream
std::ostream& operator<<(std::ostream& ostr, const basalt::status_t& status);

#endif // BASALT_STATUS_HPP
