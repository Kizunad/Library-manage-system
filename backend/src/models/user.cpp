// src/models/user.cpp

#include "models/user.hpp"
#include "utils/database_pool.hpp"
#include <exception>
#include <string>

std::unique_ptr<User> User::findById(int id){
    auto conn = DatabasePool::getInstance().getConnection();
    try {
        pqxx::work txn(*conn);
        auto result = txn.exec_params(
                "SELECT id, username, email, password_hash, role",
                "FROM users WHERE id=$1",
                id
        );

        if (result.empty()) {
            return nullptr;
        }

        auto user = User::create()
             .setUsername(result[0]["username"].as<std::string>())
             .setEmail(result[0]["email"].as<std::string>())
             .setPasswordHash(result[0]["password_hash"].as<std::string>())
             .setRole(result[0]["role"].as<std::string>())
             .build();

        user->id_= result[0]["id"].as<int>();

        txn.commit();

        return user;
    } catch (const std::exception& e) {
        //TODO: Record err log
        return nullptr;
    }
}
std::unique_ptr<User> User::findByUsername(const std::string& username){
    auto conn = DatabasePool::getInstance().getConnection();
    try {
        pqxx::work txn(*conn);
        auto result = txn.exec_params(
                "SELECT id, username, email, password_hash, role",
                "FROM users WHERE username=$1",
                username
        );

        if (result.empty()) {
            return nullptr;
        }

        auto user = User::create()
             .setUsername(result[0]["username"].as<std::string>())
             .setEmail(result[0]["email"].as<std::string>())
             .setPasswordHash(result[0]["password_hash"].as<std::string>())
             .setRole(result[0]["role"].as<std::string>())
             .build();

        user->id_= result[0]["id"].as<int>();

        txn.commit();

        return user;

    } catch (const std::exception& e) {
        //TODO: Record err log
        return nullptr;
    }
}
std::vector<std::unique_ptr<User>> User::findAll(){}

bool save();
bool update();
bool remove();
