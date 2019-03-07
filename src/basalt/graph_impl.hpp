#pragma once

#include <gsl-lite/gsl-lite.hpp>

#include <basalt/edges.hpp>
#include <basalt/graph.hpp>
#include <basalt/status.hpp>
#include <basalt/vertices.hpp>

#include "fwd.hpp"
#include "graph_kv.hpp"
#include "vertex_iterator_impl.hpp"

namespace basalt {

/// \brief Graph pointer to implementation
class GraphImpl {
  public:
    using logger_t = std::shared_ptr<spdlog::logger>;
    using column_families_t = std::vector<rocksdb::ColumnFamilyHandle*>;

    explicit GraphImpl(const std::string& path);

    inline const logger_t& logger_get() const noexcept {
        return this->logger_;
    }

    inline const std::string& path_get() const noexcept {
        return this->path_;
    }
    inline const Edges& edges_get() const noexcept {
        return this->edges_;
    }
    inline Edges& edges_get() noexcept {
        return this->edges_;
    }
    inline const Vertices& vertices_get() const noexcept {
        return this->vertices_;
    }
    inline Vertices& vertices_get() noexcept {
        return this->vertices_;
    }

    inline const db_t& db_get() const noexcept {
        return this->db_;
    }
    inline db_t& db_get() noexcept {
        return this->db_;
    }

    Status vertices_insert(basalt::vertex_t type,
                           basalt::vertex_id_t id,
                           const gsl::span<const char>& payload,
                           basalt::vertex_uid_t& vertex,
                           bool commit);

    Status vertices_insert(vertex_t type, vertex_id_t id, vertex_uid_t& vertex, bool commit);
    Status vertices_insert(const gsl::span<const vertex_t> types,
                           const gsl::span<const vertex_id_t> ids,
                           const gsl::span<const char* const> payloads,
                           const gsl::span<const std::size_t> payloads_sizes,
                           bool commit);

    Status vertices_has(const vertex_uid_t& vertex, bool& result) const;
    Status vertices_erase(const vertex_uid_t& vertex, bool commit);
    Status vertices_get(const basalt::vertex_uid_t& vertex, std::string* value);
    std::shared_ptr<VertexIteratorImpl> VertexIterator(std::size_t from) const;

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

    Status edges_has(const vertex_uid_t& vertex1, const vertex_uid_t& vertex2, bool& result) const;

    Status edges_get(const vertex_uid_t& vertex, vertex_t filter, vertex_uids_t& edges) const;

    Status edges_erase(const vertex_uid_t& vertex1, const vertex_uid_t& vertex2, bool commit);

    Status edges_erase(const vertex_uid_t& vertex, vertex_t filter, size_t& removed, bool commit);
    Status edges_erase(const vertex_uid_t& vertex, std::size_t& removed, bool commit);
    Status commit();
    std::string statistics() const;

    static Status to_status(const rocksdb::Status& status);
    static void setup_db(const rocksdb::Options& options, const std::string& path);

  private:
    Status edges_erase(rocksdb::WriteBatch& batch, const vertex_uid_t& vertex, size_t& removed);

    const std::string& path_;
    Vertices vertices_;
    Edges edges_;
    std::shared_ptr<rocksdb::Statistics> statistics_;
    std::unique_ptr<rocksdb::Options> options_;
    logger_t logger_;
    db_t db_;
    column_families_t column_families_;
    std::unique_ptr<rocksdb::ColumnFamilyHandle> vertices_column_;
    std::unique_ptr<rocksdb::ColumnFamilyHandle> edges_column_;
};

}  // namespace basalt
