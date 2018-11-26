#ifndef BASALT_NETWORK_PIMPL_HPP
#define BASALT_NETWORK_PIMPL_HPP

#include <basalt/network.hpp>
#include <basalt/status.hpp>

#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <basalt/connections.hpp>
#include <basalt/nodes.hpp>

namespace basalt {

/// \brief Network pointer to implementation
class network_pimpl_t {
  public:
    using logger_t = std::shared_ptr<spdlog::logger>;

    explicit network_pimpl_t(const std::string& path)
        : path_(path), nodes_(*this), connections_(*this) {
        if (path.empty()) {
            logger_ = spdlog::stdout_color_mt("basalt");
        } else {
            logger_ = spdlog::rotating_logger_mt(
                "basalt", path + "/logs/network.log", 1048576 * 5, 3);
        }
        logger_->info("creating or loading database at location: {}", path);
    }

    inline const logger_t& logger_get() const noexcept { return this->logger_; }

    inline const std::string& path_get() const noexcept { return this->path_; }
    inline const connections_t& connections_get() const noexcept {
        return this->connections_;
    }
    inline connections_t& connections_get() noexcept {
        return this->connections_;
    }
    inline const nodes_t& nodes_get() const noexcept { return this->nodes_; }
    inline nodes_t& nodes_get() noexcept { return this->nodes_; }

  private:
    const std::string& path_;
    nodes_t nodes_;
    connections_t connections_;
    logger_t logger_;
};
} // namespace basalt

#endif // BASALT_NETWORK_PIMPL_HPP
