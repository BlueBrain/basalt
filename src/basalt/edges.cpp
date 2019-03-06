#include <basalt/edges.hpp>

#include "graph_impl.hpp"

namespace basalt {

Edges::Edges(GraphImpl& pimpl)
    : pimpl_(pimpl) {}

Status Edges::insert(const vertex_uid_t& vertex1, const vertex_uid_t& vertex2, bool commit) {
    return pimpl_.edges_insert(vertex1, vertex2, {nullptr, 0}, commit);
}

Status Edges::insert(const vertex_uid_t& vertex1,
                     const vertex_uid_t& vertex2,
                     const char* data,
                     std::size_t size,
                     bool commit) {
    return pimpl_.edges_insert(vertex1, vertex2, {data, size}, commit);
}

Status Edges::insert(const vertex_uid_t& vertex,
                     const vertex_uids_t& vertices,
                     const std::vector<const char*>& data,
                     const std::vector<std::size_t>& sizes,
                     bool commit) {
    return pimpl_.edges_insert(vertex, vertices, data, sizes, commit);
}

Status Edges::insert(const vertex_uid_t& vertex,
                     vertex_t type,
                     const std::size_t* vertices,
                     size_t num_vertices,
                     bool create_vertices,
                     bool commit) {
    return pimpl_.edges_insert(vertex, type, {vertices, num_vertices}, create_vertices, commit);
};

Status Edges::insert(const vertex_uid_t& vertex,
                     vertex_t type,
                     const std::size_t* vertices,
                     const char* const* vertex_payloads,
                     const std::size_t* vertex_payloads_sizes,
                     size_t num_vertices,
                     bool create_vertices,
                     bool commit) {
    return pimpl_.edges_insert(vertex, type, {vertices, num_vertices},
                               {vertex_payloads, num_vertices},
                               {vertex_payloads_sizes, num_vertices}, create_vertices, commit);
};

Status Edges::has(const vertex_uid_t& vertex1, const vertex_uid_t& vertex2, bool& result) const {
    return pimpl_.edges_has(vertex1, vertex2, result);
}

Status Edges::get(const vertex_uid_t& vertex, vertex_uids_t& edges) const {
    return pimpl_.edges_get(vertex, edges);
}

Status Edges::get(const vertex_uid_t& vertex, vertex_t filter, vertex_uids_t& edges) const {
    return pimpl_.edges_get(vertex, filter, edges);
}

Status Edges::erase(const vertex_uid_t& vertex1, const vertex_uid_t& vertex2, bool commit) {
    return pimpl_.edges_erase(vertex1, vertex2, commit);
}

Status Edges::erase(const vertex_uid_t& vertex, vertex_t filter, size_t& removed, bool commit) {
    return pimpl_.edges_erase(vertex, filter, removed, commit);
}

Status Edges::erase(const vertex_uid_t& vertex, std::size_t& removed, bool commit) {
    return pimpl_.edges_erase(vertex, removed, commit);
}

}  // namespace basalt
