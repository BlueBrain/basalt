#pragma once

#include <sstream>

#include <basalt/graph.hpp>
#include <basalt/status.hpp>

namespace basalt {

template <typename Payload>
Status Vertices::insert(vertex_t type,
                        vertex_id_t id,
                        const Payload& data,
                        vertex_uid_t& res,
                        bool commit) {
    std::ostringstream oss;
    data.serialize(oss);
    // \fixme TCL get buffer beginning and length from ostringstream
    // to avoid extra std::string copy.
    const std::string raw(oss.str());
    return insert(type, id, raw.c_str(), raw.size(), res, commit);
}

template <typename T>
Status Vertices::get(const vertex_uid_t& vertex, T& payload) const {
    std::string data;
    auto const& status = get(vertex, &data);
    if (status) {
        std::istringstream istr;
        istr.rdbuf()->pubsetbuf(const_cast<char*>(data.c_str()), static_cast<long>(data.size()));
        payload.deserialize(istr);
    }
    return status;
}

}  // namespace basalt
