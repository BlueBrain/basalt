/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#pragma once

#include <gsl>

#include <basalt/edges.hpp>
#include <basalt/graph.hpp>
#include <basalt/status.hpp>
#include <basalt/vertices.hpp>

#include "config.hpp"
#include "fwd.hpp"
#include "graph_kv.hpp"

namespace basalt {

/// \brief Graph pointer to implementation
template <EdgeOrientation Orientation>
class GraphImpl {
  public:
    using logger_t = std::shared_ptr<spdlog::logger>;
    using column_families_t = std::vector<rocksdb::ColumnFamilyHandle*>;
    using edge_keys_t =
        std::array<GraphKV::edge_key_t, Orientation == EdgeOrientation::directed ? 1 : 2>;


    explicit GraphImpl(const std::string& path);
    GraphImpl(const std::string& path, Config config, bool throw_if_exists);

    inline const logger_t& logger_get() const noexcept {
        return this->logger_;
    }

    inline const std::string& path_get() const noexcept {
        return this->path_;
    }
    inline const Edges<Orientation>& edges_get() const noexcept {
        return this->edges_;
    }
    inline Edges<Orientation>& edges_get() noexcept {
        return this->edges_;
    }
    inline const Vertices<Orientation>& vertices_get() const noexcept {
        return this->vertices_;
    }
    inline Vertices<Orientation>& vertices_get() noexcept {
        return this->vertices_;
    }

    inline const db_t& db_get() const noexcept {
        return this->db_;
    }
    inline db_t& db_get() noexcept {
        return this->db_;
    }

    Status vertices_insert(const basalt::vertex_uid_t& vertex,
                           const gsl::span<const char>& payload,
                           bool commit);

    Status vertices_insert(const vertex_uid_t& vertex, bool commit);
    Status vertices_insert(const gsl::span<const vertex_t> types,
                           const gsl::span<const vertex_id_t> ids,
                           const gsl::span<const char* const> payloads,
                           const gsl::span<const std::size_t> payloads_sizes,
                           bool commit);

    Status vertices_has(const vertex_uid_t& vertex, bool& result) const;
    Status vertices_erase(const vertex_uid_t& vertex, bool commit);
    Status vertices_count(std::size_t& count) const;
    Status vertices_count(vertex_t type, std::size_t& count) const;
    Status vertices_get(const basalt::vertex_uid_t& vertex, std::string* value);
    std::shared_ptr<VertexIteratorImpl> vertex_iterator(std::size_t from) const;
    Status vertices_clear(bool commit) __attribute__((warn_unused_result));

    Status edges_insert(const vertex_uid_t& vertex1,
                        const vertex_uid_t& vertex2,
                        const gsl::span<const char>& payload,
                        bool commit);

    Status edges_insert(const vertex_uid_t& vertex,
                        const vertex_uids_t& vertices,
                        const std::vector<const char*>& data,
                        const std::vector<std::size_t>& sizes,
                        bool commit);

    Status edges_insert(const vertex_uid_t& vertex,
                        const vertex_t type,
                        const gsl::span<const vertex_id_t>& vertices,
                        bool create_vertices,
                        bool commit);

    Status edges_insert(const vertex_uid_t& vertex,
                        const vertex_t type,
                        const gsl::span<const vertex_id_t>& vertices,
                        const gsl::span<const char* const> vertex_payloads,
                        const gsl::span<const std::size_t>& vertex_payloads_sizes,
                        bool create_vertices,
                        bool commit);

    Status edges_get(const vertex_uid_t& vertex, vertex_uids_t& edges) const;
    Status edges_get(const edge_uid_t& edge, std::string* value) const
        __attribute__((warn_unused_result));

    Status edges_has(const vertex_uid_t& vertex1, const vertex_uid_t& vertex2, bool& result) const;

    Status edges_get(const vertex_uid_t& vertex, vertex_t filter, vertex_uids_t& edges) const;

    Status edges_erase(const vertex_uid_t& vertex1, const vertex_uid_t& vertex2, bool commit);

    Status edges_erase(const vertex_uid_t& vertex, vertex_t filter, size_t& removed, bool commit);
    Status edges_erase(const vertex_uid_t& vertex, std::size_t& removed, bool commit);
    Status edges_count(std::size_t& count) const;
    Status edges_clear(bool commit) __attribute__((warn_unused_result));
    std::shared_ptr<EdgeIteratorImpl> edge_iterator(std::size_t from) const;


    Status commit();
    std::string statistics() const;

    static Status to_status(const rocksdb::Status& status);

  private:
    Status edges_erase(rocksdb::WriteBatch& batch, const vertex_uid_t& vertex, size_t& removed);
    void clear(rocksdb::WriteBatch& batch,
               const std::unique_ptr<rocksdb::ColumnFamilyHandle>& handle);

    const std::string& path_;
    const Config config_;
    Vertices<Orientation> vertices_;
    Edges<Orientation> edges_;
    std::shared_ptr<rocksdb::Statistics> statistics_;
    std::unique_ptr<rocksdb::Options> options_;
    logger_t logger_;
    db_t db_;
    column_families_t column_families_;
    std::unique_ptr<rocksdb::ColumnFamilyHandle> vertices_column_;
    std::unique_ptr<rocksdb::ColumnFamilyHandle> edges_column_;
};

extern template class GraphImpl<EdgeOrientation::directed>;
extern template class GraphImpl<EdgeOrientation::undirected>;

}  // namespace basalt
