#ifndef BASALT_NETWORK_PIMPL_HPP
#define BASALT_NETWORK_PIMPL_HPP

#include <basalt/connections.hpp>
#include <basalt/network.hpp>
#include <basalt/nodes.hpp>
#include <basalt/status.hpp>

#include "fwd.hpp"
#include "graph.hpp"
#include "node_iterator_impl.hpp"

namespace basalt {

/// \brief Network pointer to implementation
class network_impl_t {
  public:
    using logger_t = std::shared_ptr<spdlog::logger>;

    explicit network_impl_t(const std::string& path);

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

    inline const db_t& db_get() const noexcept { return this->db_; }
    inline db_t& db_get() noexcept { return this->db_; }

    status_t nodes_has(const node_uid_t& node, bool& result) const;
    status_t nodes_erase(const node_uid_t& node, bool commit);
    std::shared_ptr<node_iterator_impl> node_iterator(std::size_t from) const;


    status_t connections_connect(const node_uid_t& node1,
                                 const node_uid_t& node2, const payload_t& data,
                                 bool commit = false);
    status_t connections_connect(const node_uid_t& node,
                                 const node_uids_t& nodes,
                                 const payload_t& payload, bool commit);

    status_t connections_get(const node_uid_t& node,
                             node_uids_t& connections) const;

    status_t connections_connected(const node_uid_t& node1,
                                   const node_uid_t& node2, bool& res) const;

    status_t connections_get(const node_uid_t& node, node_t filter,
                             node_uids_t& connections) const;

    status_t connections_erase(const node_uid_t& node1, const node_uid_t& node2,
                               bool commit);

    status_t connections_erase(const node_uid_t& node, node_t filter,
                               size_t& removed, bool commit);
    status_t connections_erase(const node_uid_t& node, std::size_t& removed,
                               bool commit);

    status_t commit();

    static status_t to_status(const rocksdb::Status& status);

  private:
    const std::string& path_;
    nodes_t nodes_;
    connections_t connections_;
    logger_t logger_;
    db_t db_;
};

} // namespace basalt

#endif // BASALT_NETWORK_PIMPL_HPP
