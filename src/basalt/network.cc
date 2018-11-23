#include <basalt/network.hh>

#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>


namespace basalt {

status_t::status_t(int code_, std::string message_)
    : code(code_), message(std::move(message_)) {}

static const status_t not_implemented{1, "operation-not-implemented"};

network_t::network_t(const std::string& path) : path_(path) {
    if (path.empty()) {
        logger_ = spdlog::stdout_color_mt("basalt");
    } else {
        logger_ = spdlog::rotating_logger_mt(
            "basalt", path + "/logs/network.log", 1048576 * 5, 3);
    }
    logger_->info("creating or loading database at location: {}", path);
}

status_t network_t::erase(const node_uid_t& id, bool commit) {
    logger_->debug("erase(id={}, commit={})", id, commit);
    return not_implemented;
}

status_t network_t::erase(const node_uids_t& ids, bool commit) {
    logger_->debug("erase(ids={}, commit={})", ids, commit);
    return not_implemented;
}

status_t network_t::connect(const node_uid_t& node1, const node_uid_t& node2,
                            bool commit) {
    logger_->debug("connect(node1={}, node2={}, commit={})", node1, node2,
                   commit);
    return not_implemented;
}

status_t network_t::connect(const node_uid_t& node, const node_uids_t& nodes,
                            bool commit) {
    logger_->debug("connect({}, {}, {})", node, nodes, commit);
    return not_implemented;
}

std::pair<bool, status_t> network_t::connected(const node_uid_t& node1,
                                               const node_uid_t& node2) const {
    logger_->debug("connected({}, {})", node1, node2);
    return {false, not_implemented};
}

status_t network_t::get(const node_uid_t& node,
                        node_uids_t& /*connections*/) const {
    logger_->debug("get(node={})", node);
    return not_implemented;
}

status_t network_t::get(const node_uid_t& node, node_t filter,
                        node_uids_t& /*connections*/) const {
    logger_->debug("get({}, {})", node, filter);
    return not_implemented;
}

status_t network_t::erase(const node_uid_t& node1, const node_uid_t& node2,
                          bool commit) {
    logger_->debug("erase({}, {}, {})", node1, node2, commit);
    return not_implemented;
}

status_t network_t::erase(const node_uid_t& node, node_t filter,
                          size_t& /*removed*/, bool commit) {
    logger_->debug("erase({}, {}, {})", node, filter, commit);
    return not_implemented;
}

status_t network_t::erase_connections(const node_uid_t& node, bool commit) {
    logger_->debug("erase_connections({}, {})", node, commit);
    return not_implemented;
}

status_t network_t::commit() {
    logger_->debug("commit()");
    return not_implemented;
}

network_t::node_uid_t make_id(network_t::node_t type, network_t::node_id_t id) {
    return {type, id};
}

} // namespace basalt

std::ostream& operator<<(std::ostream& ostr,
                         const basalt::network_t::node_uid_t& id) {
    return ostr << '(' << id.first << ':' << id.second << ')';
}

std::ostream& operator<<(std::ostream& ostr, const basalt::status_t& status) {
    return ostr << "code=" << status.code << " message=" << status.message;
}

std::ostream& operator<<(std::ostream& ostr,
                         const basalt::network_t::node_uids_t& ids) {
    ostr << '[';
    for (const auto& id : ids) {
        ostr << id << ' ';
    }
    return ostr << ']';
}
