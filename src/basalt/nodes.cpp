#include <limits>

#include <basalt/node_iterator.hpp>
#include <basalt/nodes.hpp>

#include "network_impl.hpp"

namespace basalt {

nodes_t::nodes_t(network_impl_t& pimpl)
    : pimpl_(pimpl) {}

status_t nodes_t::insert(basalt::node_t type,
                         basalt::node_id_t id,
                         const char* data,
                         std::size_t size,
                         basalt::node_uid_t& node,
                         bool commit) {
    return pimpl_.nodes_insert(type, id, {data, size}, node, commit);
}

status_t nodes_t::insert(basalt::node_t type,
                         basalt::node_id_t id,
                         basalt::node_uid_t& node,
                         bool commit) {
    return pimpl_.nodes_insert(type, id, node, commit);
}

status_t nodes_t::insert(const node_t* types,
                         const node_id_t* ids,
                         const char* const* payloads,
                         const std::size_t* payloads_sizes,
                         size_t num_nodes,
                         bool commit) {
    if (payloads == nullptr) {
        return pimpl_.nodes_insert({types, num_nodes}, {ids, num_nodes}, {}, {}, commit);
    }
    return pimpl_.nodes_insert({types, num_nodes}, {ids, num_nodes}, {payloads, num_nodes},
                               {payloads_sizes, num_nodes}, commit);
}


status_t nodes_t::has(const node_uid_t& node, bool& result) const {
    return pimpl_.nodes_has(node, result);
}

status_t nodes_t::get(const basalt::node_uid_t& node, std::string* value) const {
    return pimpl_.nodes_get(node, value);
}

status_t nodes_t::erase(const node_uid_t& node, bool commit) {
    return pimpl_.nodes_erase(node, commit);
}

status_t nodes_t::count(std::size_t& count) const {
    count = 0;
    return status_t::error_not_implemented();
}

node_iterator nodes_t::begin(size_t pos) const {
    return node_iterator(pimpl_, pos);
}

node_iterator nodes_t::end() const {
    return node_iterator(pimpl_, std::numeric_limits<std::size_t>::max());
}

}  // namespace basalt
