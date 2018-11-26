#ifndef BASALT_NETWORK_HPP
#define BASALT_NETWORK_HPP

#include <iosfwd>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <basalt/fwd.hpp>
#include <basalt/status.hpp>

namespace basalt {

/// \brief Undirected Connectivity Graph
class network_t {
  public:
    using node_t = int;
    using node_id_t = std::size_t;
    using node_uid_t = std::pair<node_t, std::size_t>;
    using node_uids_t = std::vector<node_uid_t>;
    using node_const_iterator_t =
        std::iterator<std::input_iterator_tag, const node_t>;

    /**
     * \brief load graph if present on disk, initialize it otherwise
     * \param path graph location on filesystem
     */
    explicit network_t(const std::string& path);
    ~network_t();

    /** \name nodes accessor
     ** \{
     */
    const nodes_t& nodes() const;
    nodes_t& nodes();
    /** \} */

    /** \name connections accessor
     ** \{
     */
    const connections_t& connections() const;
    connections_t& connections();
    /** \} */

    /**
     * \brief Process uncommitted operations
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t commit() __attribute__((warn_unused_result));

  private:
    std::unique_ptr<network_pimpl_t> pimpl_;
};

/// \brief node_id_t constructor helper function
network_t::node_uid_t make_id(network_t::node_t type, network_t::node_id_t id);

} // namespace basalt

/// \brief specialization to gently push a \a node_uid_t to an output stream
std::ostream& operator<<(std::ostream& ostr,
                         const basalt::network_t::node_uid_t& id);

/// \brief specialization to gently push a \a node_uids_t to an output stream
std::ostream& operator<<(std::ostream& ostr,
                         const basalt::network_t::node_uids_t& ids);

#endif // !BASALT_NETWORK_HPP
