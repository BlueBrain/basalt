/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#include <ostream>
#include <sstream>
#include <stdexcept>

#include <basalt/graph.hpp>
#include <basalt/status.hpp>

namespace basalt {

Status::Status(Status::Code code_, std::string message_)
    : code(code_)
    , message(std::move(message_)) {}

const Status& Status::raise_on_error() const {
    if (code != 0) {
        throw std::runtime_error(message);
    }
    return *this;
}

const Status& Status::ok() {
    static const Status ok_{Status::ok_code, ""};
    return ok_;
}

const Status& Status::error_not_implemented() {
    static const Status not_implemented_{Status::not_implemented_code, "operation-not-implemented"};
    return not_implemented_;
}

Status Status::error_missing_vertex(const vertex_uid_t& vertex) {
    std::ostringstream oss;
    oss << "Missing vertex " << vertex;
    return {missing_vertex_code, oss.str()};
}

Status Status::error_missing_edge(const edge_uid_t& edge) {
    std::ostringstream oss;
    oss << "Missing edge " << edge;
    return {missing_edge_code, oss.str()};
}

Status Status::error_invalid_edge(const basalt::vertex_uid_t& vertex1,
                                  const basalt::vertex_uid_t& vertex2) {
    std::ostringstream oss;
    oss << "Invalid edge " << vertex1 << ' ' << vertex2;
    return {invalid_edge_code, oss.str()};
}

}  // namespace basalt

std::ostream& operator<<(std::ostream& ostr, const basalt::Status& status) {
    return ostr << "code=" << status.code << " message=" << status.message;
}
