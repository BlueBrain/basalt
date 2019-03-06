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
        invalid_edge_code = -3
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
     * \return true if status is not \a ok_code, false otherwise
     */
    inline bool operator!() const noexcept {
        return code != 0;
    }
    /**
     * Implicit conversion operator
     * \return true if status is \a ok_code, false otherwise
     */
    inline explicit operator bool() const noexcept {
        return code == 0;
    }

    /**
     * Throw \a std::runtime_error if status is not \a ok_code.
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
     * \param vertex Vertex missing
     * \return A status representing a missing vertex
     */
    static Status error_missing_vertex(const vertex_uid_t& vertex);

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
