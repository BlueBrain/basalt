/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#pragma once

#include <nlohmann/json.hpp>
#include <rocksdb/db.h>

#include "fwd.hpp"


namespace basalt {

/**
 * Graph database configuration
 */
class Config {
  public:
    /**
     * \name Ctors & Dtors
     * \{
     */

    /**
     * Create default configuration
     */
    Config();

    /**
     * Read config.json file if present in database path, use
     * default configuration otherwise.
     * \param db_path path to database directory
     */
    explicit Config(const std::string& db_path);

    /**
     * Create configuration from a stream
     * \param istr input stream to read
     */
    explicit Config(std::ifstream& istr);

    /**
     * \}
     */

    /**
     * Setup RocksDB configuration option
     * \param options RocksDB configuration to update
     * \param db_path path to database
     */
    void configure(rocksdb::Options& options, const std::string& db_path) const;

    /**
     * Write configuration to an output stream in JSON format
     * \param ostr output stream to write into
     * \param indent JSON indentation size
     * \return reference to the given stream
     */
    std::ostream& write(std::ostream& ostr, std::streamsize indent = 4) const;

    /**
     * Retrieve RocksDB column families from config
     */
    std::vector<rocksdb::ColumnFamilyDescriptor> column_families() const;

    bool operator==(const Config& other) const;

    /**
     *
     * \return true if the database should be opened only for read only operations.
     */
    bool read_only() const;

  private:
    const nlohmann::json config_;
};

std::ostream& operator<<(std::ostream& ostr, const Config& config);

}  // namespace basalt
