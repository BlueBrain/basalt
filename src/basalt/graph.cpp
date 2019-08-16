/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#include <ostream>

#include <rocksdb/db.h>
#include <rocksdb/slice_transform.h>

#include <basalt/graph.hpp>

#include "graph_impl.hpp"

namespace basalt {

static Config from_file(const std::string& path) {
    std::ifstream istr;
    istr.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    istr.open(path);
    return Config(istr);
}

template <bool Ordered>
AbstractGraph<Ordered>::AbstractGraph(const std::string& path)
    : pimpl_(new GraphImpl<Ordered>(path)) {}

template <bool Ordered>
AbstractGraph<Ordered>::AbstractGraph(const std::string& path, const std::string& config)
    : pimpl_(new GraphImpl<Ordered>(path, from_file(config), true)) {}

template <bool Ordered>
AbstractGraph<Ordered>::~AbstractGraph() = default;

template <bool Ordered>
Edges<Ordered>& AbstractGraph<Ordered>::edges() {
    return pimpl_->edges_get();
}

template <bool Ordered>
Vertices<Ordered>& AbstractGraph<Ordered>::vertices() {
    return pimpl_->vertices_get();
}

template <bool Ordered>
Status AbstractGraph<Ordered>::commit() {
    return pimpl_->commit();
}

template <bool Ordered>
std::string AbstractGraph<Ordered>::statistics() const {
    return pimpl_->statistics();
}

template class AbstractGraph<true>;
template class AbstractGraph<false>;

/////

vertex_uid_t make_id(vertex_t type, vertex_id_t id) {
    return {type, id};
}

}  // namespace basalt

std::ostream& operator<<(std::ostream& ostr, const basalt::vertex_uid_t& id) {
    return ostr << '(' << id.first << ':' << id.second << ')';
}

std::ostream& operator<<(std::ostream& ostr, const basalt::vertex_uids_t& ids) {
    ostr << '[';
    for (const auto& id: ids) {
        ostr << id << ' ';
    }
    return ostr << ']';
}

std::ostream& operator<<(std::ostream& ostr, const basalt::edge_uid_t& id) {
    return ostr << id.first << "->" << id.second;
}
