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

Graph::Graph(const std::string& path)
    : pimpl_(new GraphImpl(path)) {}

Graph::Graph(const std::string& path, const std::string& config)
    : pimpl_(new GraphImpl(path, from_file(config), true)) {}

Graph::~Graph() = default;

Edges& Graph::edges() {
    return pimpl_->edges_get();
}

Vertices& Graph::vertices() {
    return pimpl_->vertices_get();
}

Status Graph::commit() {
    return pimpl_->commit();
}

std::string Graph::statistics() const {
    return pimpl_->statistics();
}

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
