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
    /** \brief iterator over vertices */
    using node_const_iterator_t = std::iterator<std::input_iterator_tag, const node_t>;

    /**
     * \brief load graph if present on disk, initialize it otherwise
     * \param path graph location on filesystem
     */
    explicit network_t(const std::string& path);
    ~network_t();

    /** \name connections accessor
     ** \{ */
    /** \brief edges accessor */
    connections_t& connections();

    /** \brief vertices accessor */
    nodes_t& nodes();
    /** \} */

    /**
     * \brief Process uncommitted operations
     * \return information whether operation succeeded or not
     */
    status_t commit() __attribute__((warn_unused_result));

    /**
     * \brief Provides human readable string of all database counters
     */
    std::string statistics() const;

  private:
    std::unique_ptr<network_impl_t> pimpl_;
};

/// \brief node_id_t constructor helper function
node_uid_t make_id(node_t type, node_id_t id);

}  // namespace basalt

/// \brief specialization to gently push a \a node_uid_t to an output stream
std::ostream& operator<<(std::ostream& ostr, const basalt::node_uid_t& id);

/// \brief specialization to gently push a \a node_uids_t to an output stream
std::ostream& operator<<(std::ostream& ostr, const basalt::node_uids_t& ids);

#endif  // !BASALT_NETWORK_HPP
