#include <fstream>
#include <iomanip>

#include <gsl-lite/gsl-lite.hpp>
#include <rocksdb/db.h>
#include <rocksdb/filter_policy.h>
#include <rocksdb/slice_transform.h>
#include <rocksdb/statistics.h>
#include <rocksdb/table.h>

#include "config.hpp"
#include <basalt/status.hpp>
#include <rocksdb/table.h>

namespace basalt {

static inline Status to_status(const rocksdb::Status& status) {
    return {static_cast<Status::Code>(status.code()), status.ToString()};
}

static inline std::string column_family_name(const std::string& name) {
    if (name == "<default>") {
        return rocksdb::kDefaultColumnFamilyName;
    } else {
        return name;
    }
}

static const rocksdb::SliceTransform* fixed_prefix_extractor(const nlohmann::json& config) {
    return rocksdb::NewFixedPrefixTransform(config["prefix_len"].get<size_t>());
}

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

static std::shared_ptr<const rocksdb::FilterPolicy> filter_policy(const nlohmann::json& config) {
    auto const type = config["type"].get<std::string>();
    if (type == "bloom") {
        return bloom_filter_policy(config["config"]);
    } else {
        throw std::runtime_error("Unknown filter policy type=" + type);
    }
}

static std::shared_ptr<rocksdb::TableFactory> block_based_table_factory(
    const nlohmann::json& config) {
    rocksdb::BlockBasedTableOptions options;
    auto const& fpc = config.find("filter_policy");
    if (fpc != config.end()) {
        options.filter_policy = filter_policy(fpc.value());
    }
    return std::shared_ptr<rocksdb::TableFactory>(rocksdb::NewBlockBasedTableFactory(options));
}

static rocksdb::ColumnFamilyOptions column_families_options(const nlohmann::json& config) {
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
                result.table_factory = block_based_table_factory(tfc.value()["config"]);
            } else {
                throw std::runtime_error("Unknown table factory type=" + type);
            }
        }
    }

    return result;
}

static void create_columns_families(const nlohmann::json& config,
                                    const std::string& db_path,
                                    rocksdb::Options& options) {
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
            auto const& cf_options = column_families_options(cf_config["config"]);
            to_status(db_ptr->CreateColumnFamily(cf_options, name, &cf)).raise_on_error();
            delete cf;
        }
    }
}

static void setup_statistics(const nlohmann::json& config, rocksdb::Options& options) {
    auto const& stats = config.find("statistics");
    if (stats != config.end()) {
        if (stats.value().get<bool>()) {
            options.statistics = rocksdb::CreateDBStatistics();
        }
    }
}

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

static nlohmann::json default_json() {
    nlohmann::json config;
    config["statistics"] = true;
    config["max_open_files"] = -1;
    config["create_if_missing"] = true;
    config["column_families"] = {
        {{"name", "<default>"},
         {"config",
          {{"write_buffer_size", 128u << 20u /* 128MB */},
           {"target_file_size_base", 128u << 20u /* 128MB */},
           {"max_bytes_for_level_base", 10 * 128u << 20u /* 128MB */},
           {"prefix_extractor", {{"type", "fixed"}, {"config", {{"prefix_len", 5}}}}}}}},
        {{"name", "edges"},
         {"config",
          {{"write_buffer_size", 128u << 20u /* 128MB */},
           {"target_file_size_base", 128u << 20u /* 128MB */},
           {"max_bytes_for_level_base", 10 * 128u << 20u /* 1280MB */},
           {"prefix_extractor", {{"type", "fixed"}, {"config", {{"prefix_len", 13}}}}},
           {"table_factory",
            {{"type", "block-based"},
             {"config",
              {{"filter-policy",
                {{"type", "bloom"},
                 {"config",
                  {
                      {"bits_per_key", 10},
                      {"use_block_based_builder", true},
                  }}}},
               {"index", {"binary", "hash"}},
               {"block_size", 4096}}}}}}}}};
    return config;
}


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
    } else {
        return default_json();
    }
}

Config::Config(const std::string& db_path)
    : config_(from_db_path(db_path)) {}

Config::Config()
    : config_(default_json()) {}

Config::Config(std::ifstream& istr)
    : config_(from_stream(istr)) {}

void Config::configure(rocksdb::Options& options, const std::string& db_path) const {
    setup_statistics(this->config_, options);
    setup_max_open_files(this->config_, options);
    setup_create_if_missing(this->config_, options);
    create_columns_families(this->config_["column_families"], db_path, options);
}

std::ostream& Config::write(std::ostream& ostr, std::streamsize indent) const {
    const auto width = static_cast<int>(ostr.width());
    return ostr << std::setw(static_cast<int>(indent)) << this->config_ << std::setw(width);
}

std::vector<rocksdb::ColumnFamilyDescriptor> Config::column_families() const {
    std::vector<rocksdb::ColumnFamilyDescriptor> cfd;
    for (auto const& cf_config: this->config_["column_families"]) {
        auto const& name = column_family_name(cf_config["name"]);
        auto const& cf_options = column_families_options(cf_config["config"]);
        cfd.emplace_back(name, cf_options);
    }
    return cfd;
}

bool Config::operator==(const Config& other) const {
    return this->config_ == other.config_;
}

std::ostream& operator<<(std::ostream& ostr, const Config& config) {
    return config.write(ostr);
}

}  // namespace basalt
