#include <limits>

#include <basalt/vertex_iterator.hpp>
#include <basalt/vertices.hpp>

#include "graph_impl.hpp"

namespace basalt {

Vertices::Vertices(GraphImpl& pimpl)
    : pimpl_(pimpl) {}

Status Vertices::insert(basalt::vertex_t type,
                        basalt::vertex_id_t id,
                        const char* data,
                        std::size_t size,
                        basalt::vertex_uid_t& vertex,
                        bool commit) {
    return pimpl_.vertices_insert(type, id, {data, size}, vertex, commit);
}

Status Vertices::insert(basalt::vertex_t type,
                        basalt::vertex_id_t id,
                        basalt::vertex_uid_t& vertex,
                        bool commit) {
    return pimpl_.vertices_insert(type, id, vertex, commit);
}

Status Vertices::insert(const vertex_t* types,
                        const vertex_id_t* ids,
                        const char* const* payloads,
                        const std::size_t* payloads_sizes,
                        size_t num_vertices,
                        bool commit) {
    if (payloads == nullptr) {
        return pimpl_.vertices_insert({types, num_vertices}, {ids, num_vertices}, {}, {}, commit);
    }
    return pimpl_.vertices_insert({types, num_vertices}, {ids, num_vertices},
                                  {payloads, num_vertices}, {payloads_sizes, num_vertices}, commit);
}


Status Vertices::has(const vertex_uid_t& vertex, bool& result) const {
    return pimpl_.vertices_has(vertex, result);
}

Status Vertices::get(const basalt::vertex_uid_t& vertex, std::string* value) const {
    return pimpl_.vertices_get(vertex, value);
}

Status Vertices::erase(const vertex_uid_t& vertex, bool commit) {
    return pimpl_.vertices_erase(vertex, commit);
}

Status Vertices::count(std::size_t& count) const {
    count = 0;
    return Status::error_not_implemented();
}

VertexIterator Vertices::begin(size_t position) const {
    return VertexIterator(pimpl_, position);
}

VertexIterator Vertices::end() const {
    return VertexIterator(pimpl_, std::numeric_limits<std::size_t>::max());
}

}  // namespace basalt
