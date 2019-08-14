/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#pragma once

#include <fstream>
#include <iosfwd>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <basalt/fwd.hpp>
#include <basalt/status.hpp>

namespace basalt {

/**
 * \brief Undirected Connectivity Graph
 */
class Graph {
  public:
    /** \brief iterator over vertices */
    using vertex_const_iterator_t = std::iterator<std::input_iterator_tag, const vertex_t>;

    /**
     * \name Ctors & Dtor
     * \{
     */

    /**
     * \brief load graph if present on disk, initialize it otherwise
     * \param path graph directory on disk
     */
    explicit Graph(const std::string& path);

    /**
     * \brief create graph on disk at the given path with the given configuration
     * \param path the graph directory on disk (must not exist)
     * \param config the path to a JSON file
     */
    Graph(const std::string& path, const std::string& config);

    ~Graph();

    /**
     * \}
     */

    /**
     * \name edges accessor
     * \{
     */

    /**
     * \brief edges accessor
     */
    Edges& edges();

    /**
     * \brief vertices accessor
     */
    Vertices& vertices();

    /**
     * \}
     * */

    /**
     * \brief Process uncommitted operations
     * \return information whether operation succeeded or not
     */
    Status commit() __attribute__((warn_unused_result));

    /**
     * \brief Provides human readable string of all database counters
     */
    std::string statistics() const;

  private:
    std::unique_ptr<GraphImpl> pimpl_;
};

/**
 * \brief \a vertex_uid_t constructor helper function
 */
vertex_uid_t make_id(vertex_t type, vertex_id_t id);

}  // namespace basalt

/**
 * \brief specialization to gently push a \a vertex_uid_t to an output stream
 */
std::ostream& operator<<(std::ostream& ostr, const basalt::vertex_uid_t& id);

/**
 * \brief specialization to gently push a \a vertex_uids_t to an output stream
 */
std::ostream& operator<<(std::ostream& ostr, const basalt::vertex_uids_t& ids);

/**
 * \brief specialization to gently push a \a edge_uid_t to an output stream
 */
std::ostream& operator<<(std::ostream& ostr, const basalt::edge_uid_t& id);
