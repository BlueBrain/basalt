#pragma once

#include <cstddef>
#include <utility>
#include <vector>

namespace basalt {

/// Forward declarations
class connections_t;
class network_impl_t;
class node_iterator_impl;
class node_iterator;
class nodes_t;

using node_t = int;
using node_id_t = std::size_t;
using node_uid_t = std::pair<node_t, std::size_t>;
using node_uids_t = std::vector<node_uid_t>;

}  // namespace basalt
