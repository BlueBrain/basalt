/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#pragma once

#include <iosfwd>
#include <string>

#include <basalt/fwd.hpp>

namespace basalt {

/**
 * Result of one or several graph operations
 */
struct Status {
    /**
     * Operation status code
     */
    enum Code {
        ok_code = 0,
        not_implemented_code = -1,
        missing_vertex_code = -2,
        invalid_edge_code = -3,
        missing_edge_code = -4,
    };


    /**
     * \name Ctors and dtors.
     * \{
     */

    /**
     * \brief Construct a Status
     * \param code status code
     * \param message message for humans
     */
    Status(Code code, std::string message);
    Status() = delete;
    /** \} */

    /**
     * Negation operator
     * \return true if status is not \a Code::ok_code, false otherwise
     */
    inline bool operator!() const noexcept {
        return code != 0;
    }
    /**
     * Implicit conversion operator
     * \return true if status is \a Code::ok_code, false otherwise
     */
    inline explicit operator bool() const noexcept {
        return code == 0;
    }

    /**
     * \exception std::runtime_error whenever status is not \a Code::ok_code.
     * \return this instance if member does not throw
     */
    const Status& raise_on_error() const;

    /**
     * \brief Construct a \a Status
     * \return A status representing a not implemented state
     */
    static const Status& error_not_implemented();

    /**
     * \brief Construct a \a Status
     * \param vertex The missing vertex
     * \return A status representing a missing vertex
     */
    static Status error_missing_vertex(const vertex_uid_t& vertex);

    /**
     * \brief Construct a \a Status
     * \param edge The missing edge
     * \return A \a Status representing the missing edge
     */
    static Status error_missing_edge(const edge_uid_t& edge);

    /**
     * \brief Construct a \a Status
     * \param vertex1 one end of an edge
     * \param vertex2 other end of the edge
     * \return A status representing a invalid edge between 2 vertices
     */
    static Status error_invalid_edge(const vertex_uid_t& vertex1, const vertex_uid_t& vertex2);

    /**
     * \brief Create Status
     * \return A status representing a situation where everything is alright.
     */
    static const Status& ok();

    /// status code
    const Code code;
    /// status message for humans
    const std::string message;
};

}  // namespace basalt

/// \brief specialization to gently push a \a Status to an output stream
std::ostream& operator<<(std::ostream& ostr, const basalt::Status& status);
