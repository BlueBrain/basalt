/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <gsl>
#include <rocksdb/cache.h>
#include <rocksdb/db.h>
#include <rocksdb/filter_policy.h>
#include <rocksdb/slice_transform.h>
#include <rocksdb/statistics.h>
#include <rocksdb/table.h>

#include <basalt/status.hpp>

#include "config.hpp"
#include "system.hpp"

namespace basalt {

static void compression_options(const nlohmann::json& config,
                                rocksdb::CompressionOptions& options) {
    auto window_bits = config.find("window_bits");
    if (window_bits != config.end()) {
        window_bits.value().get_to(options.window_bits);
    }
    auto level = config.find("level");
    if (level != config.end()) {
        level.value().get_to(options.level);
    }
    auto strategy = config.find("strategy");
    if (strategy != config.end()) {
        strategy.value().get_to(options.strategy);
    }
}

static rocksdb::CompressionType compression_type(std::string name) {
    rocksdb::CompressionType result;
    std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) {
        return std::toupper(c);
    });
    if (name == "NO" || name.empty()) {
        result = rocksdb::CompressionType::kNoCompression;
    } else if (name == "SNAPPY") {
        result = rocksdb::CompressionType::kSnappyCompression;
    } else if (name == "ZLIB") {
        result = rocksdb::CompressionType::kZlibCompression;
    } else if (name == "LZ4") {
        result = rocksdb::CompressionType::kLZ4Compression;
    } else if (name == "LZ4HC") {
        result = rocksdb::CompressionType::kLZ4HCCompression;
    } else {
        std::ostringstream iss;
        iss << "Unsupported compression format: '" << name << '\'';
        throw std::runtime_error(iss.str());
    }
    return result;
}


static void configure_compression(
    const nlohmann::json& config,
    std::pair<rocksdb::CompressionType, rocksdb::CompressionOptions>& compression) {
    compression.first = compression_type(config["type"].get<std::string>());
    auto json_options = config.find("config");
    if (json_options != config.end()) {
        compression_options(json_options.value(), compression.second);
    }
}

static std::unique_ptr<std::pair<rocksdb::CompressionType, rocksdb::CompressionOptions>>
compression_if_present(const nlohmann::json& config) {
    auto compression = config.find("compression");
    if (compression != config.end()) {
        std::unique_ptr<std::pair<rocksdb::CompressionType, rocksdb::CompressionOptions>> result(
            new std::pair<rocksdb::CompressionType, rocksdb::CompressionOptions>);
        configure_compression(compression.value(), *result);
        return result;
    }
    return nullptr;
}

/**
 * Convert rocksdb status object to a basalt one
 */
static inline Status to_status(const rocksdb::Status& status) {
    return {static_cast<Status::Code>(status.code()), status.ToString()};
}

/**
 * Get real column family name from one specified in the JSON config
 */
static inline std::string column_family_name(const std::string& name) {
    if (name == "<default>") {
        return rocksdb::kDefaultColumnFamilyName;
    }
    return name;
}

/**
 * Get a fixed-prefix slice transform rocksdb configuration from JSON config
 */
static const rocksdb::SliceTransform* fixed_prefix_extractor(const nlohmann::json& config) {
    return rocksdb::NewFixedPrefixTransform(config["prefix_len"].get<size_t>());
}

/**
 * \brief parse human readable number of bytes
 * \param capacity valid string: "42", "1024k", "2M", "10T",
 *        "10%" (means 10% of total usable main memory)
 * \return number of bytes
 */
static size_t capacity_from_string(const std::string& capacity) {
    if (capacity.size() < 2) {
        throw std::runtime_error("Invalid lru cache capacity. Expecting at least 2 characters: '" +
                                 capacity + '\'');
    }
    auto size_len = capacity.size();
    size_t factor = 1;
    if (std::isdigit(capacity.back()) != 0) {
        size_len -= 1;
        auto unit = capacity.back();
        if (unit == 't' || unit == 'T') {
            factor <<= 40u;
        } else if (unit == 'g' || unit == 'G') {
            factor <<= 30u;
        } else if (unit == 'm' || unit == 'M') {
            factor <<= 20u;
        } else if (unit == 'k' || unit == 'K') {
            factor <<= 10u;
        } else if (unit == '%') {
            size_t percent;
            std::istringstream iss(capacity.substr(0, size_len));
            iss >> percent;

            return system::available_memory_bytes() / percent;
        } else {
            throw std::runtime_error(std::string("Unknown unit: ") + unit);
        }
    }
    size_t size;
    std::istringstream iss(capacity.substr(0, size_len));
    iss >> size;
    return size * factor;
}

/**
 * Get least recent use block cache rocksdb config from JSON config
 */
static std::shared_ptr<rocksdb::Cache> lru_block_cache(const nlohmann::json& config) {
    int num_shard_bits = 4;
    {
        auto num_shard_bits_json = config.find("num_shard_bits");
        if (num_shard_bits_json != config.end()) {
            num_shard_bits_json.value().get_to(num_shard_bits);
        }
    }
    auto capacity_json = config["capacity"];
    size_t capacity;
    if (capacity_json.is_string()) {
        capacity = capacity_from_string(capacity_json.get<std::string>());
    } else if (capacity_json.is_number()) {
        capacity_json.get_to(capacity);
    } else {
        throw std::runtime_error(
            "Unexpected type for lru cache capacity. Expecting either string or number");
    }

    return rocksdb::NewLRUCache(capacity, num_shard_bits);
}

/**
 * Get rocksdb block cache config from a JSON config
 * \param config JSON config to read
 * \param global_block_cache the block cache declared at top-level of the JSON config
 */
static std::shared_ptr<rocksdb::Cache> block_cache(
    const nlohmann::json& config,
    std::shared_ptr<rocksdb::Cache>& global_block_cache) {
    auto const type = config["type"].get<std::string>();
    if (type == "global") {
        if (global_block_cache == nullptr) {
            throw std::runtime_error("Global block cache is undefined");
        }
        return global_block_cache;
    }
    if (type == "lru") {
        return lru_block_cache(config["config"]);
    }
    throw std::runtime_error(std::string("Unknown block cache type: ") + type);
}

/**
 * Get rocksdb block cache from a parent JSON config having a "block_cache" entry.
 * \param config JSON config
 * \param global_block_cache the block cache declared at top-level of the JSON config
 */
static std::shared_ptr<rocksdb::Cache> block_cache_if_present(
    const nlohmann::json& config,
    std::shared_ptr<rocksdb::Cache>& global_block_cache) {
    std::shared_ptr<rocksdb::Cache> result;
    auto lru_cache_config = config.find("block_cache");
    if (lru_cache_config != config.end()) {
        result = block_cache(lru_cache_config.value(), global_block_cache);
    }
    return result;
}

/**
 * Get rocksdb bloom filter policy config from JSON config
 */
static std::shared_ptr<const rocksdb::FilterPolicy> bloom_filter_policy(
    const nlohmann::json& config) {
    bool use_block_based_builder = true;
    const auto& ubbb = config.find("use_block_based_builder");
    if (ubbb != config.end()) {
        use_block_based_builder = ubbb.value().get<bool>();
    }
    const auto bits_per_key = config["bits_per_key"].get<int>();
    return std::shared_ptr<const rocksdb::FilterPolicy>(
        rocksdb::NewBloomFilterPolicy(bits_per_key, use_block_based_builder));
}

/**
 * get rocksdb filter policy config from JSON config
 */
static std::shared_ptr<const rocksdb::FilterPolicy> filter_policy(const nlohmann::json& config) {
    auto const type = config["type"].get<std::string>();
    if (type == "bloom") {
        return bloom_filter_policy(config["config"]);
    }
    throw std::runtime_error("Unknown filter policy type=" + type);
}

/**
 * Get the rocksdb block-based table factory (the only table factory supported actually)
 * from a JSON config
 */
static std::shared_ptr<rocksdb::TableFactory> block_based_table_factory(
    const nlohmann::json& config,
    std::shared_ptr<rocksdb::Cache> global_block_cache) {
    rocksdb::BlockBasedTableOptions options;
    options.block_cache = block_cache_if_present(config, global_block_cache);
    auto const& fpc = config.find("filter_policy");
    if (fpc != config.end()) {
        options.filter_policy = filter_policy(fpc.value());
    }
    return std::shared_ptr<rocksdb::TableFactory>(rocksdb::NewBlockBasedTableFactory(options));
}

/**
 * Get the list of rocksdb column families configuration from JSON config
 */
static rocksdb::ColumnFamilyOptions column_families_options(
    const nlohmann::json& config,
    std::shared_ptr<rocksdb::Cache>& global_block_cache) {
    rocksdb::ColumnFamilyOptions result;
    result.write_buffer_size =
        config["write_buffer_size"].get<decltype(result.write_buffer_size)>();
    result.target_file_size_base =
        config["target_file_size_base"].get<decltype(result.target_file_size_base)>();
    result.max_bytes_for_level_base =
        config["max_bytes_for_level_base"].get<decltype(result.max_bytes_for_level_base)>();
    {
        auto const& pec = config.find("prefix_extractor");
        if (pec != config.end()) {
            auto const type = pec.value()["type"].get<std::string>();
            if (type == "fixed") {
                result.prefix_extractor.reset(fixed_prefix_extractor(pec.value()["config"]));
            } else {
                throw std::runtime_error("Unknown prefix_extractor type=" + type);
            }
        }
    }
    {
        auto const& tfc = config.find("table_factory");
        if (tfc != config.end()) {
            auto const type = tfc.value()["type"].get<std::string>();
            if (type == "block-based") {
                result.table_factory = block_based_table_factory(tfc.value()["config"],
                                                                 global_block_cache);
            } else {
                throw std::runtime_error("Unknown table factory type=" + type);
            }
        }
    }

    return result;
}

/**
 * Prepare the rocksdb column families on the filesystem if they do not already exist.
 */
static void create_columns_families(const nlohmann::json& config,
                                    const std::string& db_path,
                                    rocksdb::Options& options,
                                    std::shared_ptr<rocksdb::Cache> global_block_cache) {
    std::unique_ptr<rocksdb::DB> db_ptr;
    {  // open db
        rocksdb::DB* db;
        rocksdb::Status status = rocksdb::DB::Open(options, db_path, &db);
        db_ptr.reset(db);
        if (status.code() == rocksdb::Status::kInvalidArgument) {
            return;
        }
        to_status(status).raise_on_error();
    }

    std::vector<std::string> column_family_names;
    rocksdb::DB::ListColumnFamilies(options, db_ptr->GetName(), &column_family_names);

    for (auto const& cf_config: config) {
        auto const& name = column_family_name(cf_config["name"]);
        if (std::find(column_family_names.begin(), column_family_names.end(), name) ==
            column_family_names.end()) {
            gsl::owner<rocksdb::ColumnFamilyHandle*> cf = nullptr;
            auto const& cf_options = column_families_options(cf_config["config"],
                                                             global_block_cache);
            to_status(db_ptr->CreateColumnFamily(cf_options, name, &cf)).raise_on_error();
            delete cf;
        }
    }
}

/**
 * Setup rocksdb statistics according to JSON config
 */
static void setup_statistics(const nlohmann::json& config, rocksdb::Options& options) {
    auto const& stats = config.find("statistics");
    if (stats != config.end()) {
        if (stats.value().get<bool>()) {
            options.statistics = rocksdb::CreateDBStatistics();
        }
    }
}

/**
 * Setup rocksdb maximum number of opened files according to JSON config
 */
static void setup_max_open_files(const nlohmann::json& config, rocksdb::Options& options) {
    auto const& value = config.find("max_open_files");
    if (value != config.end()) {
        options.max_open_files = value.value().get<decltype(options.max_open_files)>();
    }
}

static void setup_create_if_missing(const nlohmann::json& config, rocksdb::Options& options) {
    auto const& value = config.find("create_if_missing");
    if (value != config.end()) {
        options.create_if_missing = value.value().get<decltype(options.create_if_missing)>();
    }
}

static void setup_compression(const nlohmann::json& config, rocksdb::Options& options) {
    auto compression = compression_if_present(config);
    if (compression != nullptr) {
        options.compression = compression->first;
        options.compression_opts = compression->second;
    }
}

/**
 * Provide the default JSON config if not provided when creating the database
 */
static nlohmann::json default_json() {
    nlohmann::json config;
    config["read_only"] = false;
    config["statistics"] = true;
    config["max_open_files"] = -1;
    config["create_if_missing"] = true;
    // clang-format off
    config["block_cache"] = {
        {"type", "lru"},
        {"config", {
            {"capacity", 1u << 30u /* 1GB */},
            {"num_shard_bits", 4}
        }}
    };
    config["compression"] = {
        {"type", "snappy"}
    };
    config["column_families"] = {
        {
            {"name", "<default>"},
            {"config", {
                {"write_buffer_size", 128u << 20u /* 128MB */},
                {"target_file_size_base", 128u << 20u /* 128MB */},
                {"max_bytes_for_level_base", 10 * 128u << 20u /* 1280MB */},
                {"prefix_extractor", {
                    {"type", "fixed"},
                    {"config", {
                        {"prefix_len", 5}
                    }}
                }},
                {"table_factory", {
                    {"type", "block-based"},
                    {"config", {
                        {"block_cache", {
                            {"type", "global"}
                        }}
                    }}
                }}
            }}
        },
        {
            {"name", "edges"},
            {"config", {
                {"write_buffer_size", 128u << 20u /* 128MB */},
                {"target_file_size_base", 128u << 20u /* 128MB */},
                {"max_bytes_for_level_base", 10 * 128u << 20u /* 1280MB */},
                {"prefix_extractor", {
                    {"type", "fixed"},
                    {"config", {
                        {"prefix_len", 13}
                    }}
                }},
                {"table_factory", {
                    {"type", "block-based"},
                    {"config", {
                        {"block_cache", {
                            {"type", "lru"},
                            {"config", {
                                {"capacity", "10%"}
                            }}
                        }},
                        {"filter-policy", {
                            {"type", "bloom"},
                            {"config", {
                                {"bits_per_key", 10},
                                {"use_block_based_builder", true},
                            }}
                        }},
                        {"index", {"binary", "hash"}},
                        {"block_size", 4096}
                    }}
                }}
            }}
        }
    };
    // clang-format on
    return config;
}


/**
 * Get JSON config from an input stream
 */
static nlohmann::json from_stream(std::ifstream& istr) {
    nlohmann::json config;
    istr >> config;
    return config;
}

/**
 * Read configuration file if present in database path, provides
 * default configuration otherwise.
 * \param db_path directory path to database
 * \return JSON configuration
 */
static nlohmann::json from_db_path(const std::string& db_path) {
    auto json_file = db_path + "/config.json";
    std::ifstream istr;
    istr.open(json_file);
    if (istr.is_open()) {
        return from_stream(istr);
    }
    return default_json();
}

Config::Config(const std::string& db_path)
    : config_(from_db_path(db_path)) {}

Config::Config()
    : config_(default_json()) {}

Config::Config(std::ifstream& istr)
    : config_(from_stream(istr)) {}

void Config::configure(rocksdb::Options& options, const std::string& db_path) const {
    setup_statistics(config_, options);
    setup_max_open_files(config_, options);
    setup_create_if_missing(config_, options);
    setup_compression(config_, options);

    std::shared_ptr<rocksdb::Cache> empty;
    auto global_block_cache = block_cache_if_present(config_, empty);
    create_columns_families(config_["column_families"], db_path, options, global_block_cache);
}

std::ostream& Config::write(std::ostream& ostr, std::streamsize indent) const {
    const auto width = static_cast<int>(ostr.width());
    return ostr << std::setw(static_cast<int>(indent)) << config_ << std::setw(width);
}

std::vector<rocksdb::ColumnFamilyDescriptor> Config::column_families() const {
    std::vector<rocksdb::ColumnFamilyDescriptor> cfd;
    std::shared_ptr<rocksdb::Cache> empty;
    auto global_block_cache = block_cache_if_present(config_, empty);
    for (auto const& cf_config: config_["column_families"]) {
        auto const& name = column_family_name(cf_config["name"]);
        auto const& cf_options = column_families_options(cf_config["config"], global_block_cache);
        cfd.emplace_back(name, cf_options);
    }
    return cfd;
}

bool Config::read_only() const {
    bool read_only = false;
    auto config = config_.find("read_only");
    if (config != config_.end()) {
        read_only = config.value().get<bool>();
    }
    return read_only;
}

bool Config::operator==(const Config& other) const {
    return config_ == other.config_;
}

std::ostream& operator<<(std::ostream& ostr, const Config& config) {
    return config.write(ostr);
}

}  // namespace basalt
