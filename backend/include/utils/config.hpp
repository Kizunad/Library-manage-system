// include/utils/config.hpp

#pragma once
#include <string>
#include <unordered_map>

class Config {
public:
    static Config& getInstance() {
        static Config instance;
        return instance;
    }

    std::string get(const std::string& key) const {
        auto iter = config_.find(key);
        return iter != config_.end() ? iter->second : "" ;
    }

private:
    Config() {
        loadConfig();
    }

    void loadConfig() {
        config_["DB_NAME"] = "library_db";
        config_["DB_USER"] = "postgres";
        config_["DB_PASSWORD"] = "password";
        config_["DB_HOST"] = "localhost";
        config_["DB_PORT"] = "5432";
    }
std::unordered_map<std::string , std::string> config_;


};
