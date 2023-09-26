#pragma once

#include <tomlcpp.hpp>
#include <fmt/core.h>
#include "spdlog/spdlog.h"
#include <filesystem>


namespace skylight {

    typedef std::shared_ptr<toml::Table> Config;

    inline std::shared_ptr<toml::Table> GetConfig(std::string filename,
                                                  std::vector<std::filesystem::path> configSearchPaths = {
                                                          "/usr/local/etc/"}) {


        for (std::filesystem::path &searchPath: configSearchPaths) {
            std::filesystem::path filePath = searchPath / filename;
            toml::Result result = toml::parseFile(filePath.string());
            if (result.table) {
                spdlog::info("loaded {}", filePath.string());
                return result.table;
            }
        }

        throw std::runtime_error("Failed to find config");

    }

    // some wrappers to make this less gross

    inline bool getConfigBool(Config config, std::string key) {
        auto [ok, value] = config->getBool(key);
        if (!ok) {
            throw std::runtime_error(fmt::format("failed to find {} in config file", key));
        }
        return value;
    }

    inline int64_t getConfigInt(Config config, std::string key) {
        auto [ok, value] = config->getInt(key);
        if (!ok) {
            throw std::runtime_error(fmt::format("failed to find {} in config file", key));
        }
        return value;
    }

    inline double getConfigDouble(Config config, std::string key) {
        auto [ok, value] = config->getDouble(key);
        if (!ok) {
            throw std::runtime_error(fmt::format("failed to find {} in config file", key));
        }
        return value;
    }

    inline std::string getConfigString(Config config, std::string key) {
        auto [ok, value] = config->getString(key);
        if (!ok) {
            throw std::runtime_error(fmt::format("failed to find {} in config file", key));
        }
        return value;
    }


}


