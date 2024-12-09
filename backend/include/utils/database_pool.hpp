// include/utils/database_pool.hpp

#pragma once
#include <condition_variable>
#include <memory.h>
#include <memory>
#include <queue>
#include <mutex>
#include <pqxx/pqxx>
#include "utils/config.hpp"

#define MAX_CONNECTIONS 10
class DatabasePool {
public:
    static DatabasePool& getInstance() {
        static DatabasePool instance;
        return instance;
    }

    /*
    Get the connetion ptr for the database
    */
    std::shared_ptr<pqxx::connection> getConnection() {
        std::unique_lock<std::mutex> lock(mutex_);

        while (connections_.empty() && connections_count_ >= max_connections_) {
            conn_available_.wait(lock);
        }

        if (!connections_.empty()) {
            auto conn = connections_.front();
            connections_.pop();
            return conn;
        }

        auto conn = CreateConnection();
        connections_count_++;
        return conn;
    }

    /*
    Release connection ptr to the pool, param is the connection ptr
    */
    void releaseConnection(const std::shared_ptr<pqxx::connection>& conn) {
        std::unique_lock<std::mutex> lock(mutex_);
        connections_.push(conn);
        conn_available_.notify_one();
    }
private:
    std::queue<std::shared_ptr<pqxx::connection>> connections_;
    std::mutex mutex_;
    std::condition_variable conn_available_;
    const int max_connections_{MAX_CONNECTIONS};
    int connections_count_{0};


    DatabasePool() {
        for (int i = 0; i < MAX_CONNECTIONS; i++) {
            connections_.push(CreateConnection());
            connections_count_++;
        }
    }

    static std::shared_ptr<pqxx::connection> CreateConnection() {
        Config& config = Config::getInstance();
        std::string conn_str =
            "dbname=" + config.get("DB_NAME") +
            " user=" + config.get("DB_USER") +
            " password" + config.get("DB_HOST") +
            " port=" + config.get("DB_PORT");

        return std::make_shared<pqxx::connection>(conn_str);
    }
};
