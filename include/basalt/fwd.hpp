#ifndef BASALT_FWD_HPP
#define BASALT_FWD_HPP

#include <cstddef>
#include <utility>
#include <vector>

namespace basalt {

/// Forward declarations
class connections_t;
class network_impl_t;
class node_iterator;
class nodes_t;

using node_t = int;
using node_id_t = std::size_t;
using node_uid_t = std::pair<node_t, std::size_t>;
using node_uids_t = std::vector<node_uid_t>;
using payload_t = std::vector<char>;

} // namespace basalt

#endif // BASALT_FWD_HPP
