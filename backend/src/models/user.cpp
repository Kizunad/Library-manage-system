// src/models/user.cpp

#include "models/user.hpp"
#include "utils/database_pool.hpp"
#include <exception>
#include <memory>
#include <string>
#include <vector>

std::unique_ptr<User> User::findById(int id){
    auto conn = DatabasePool::getInstance().getConnection();
    try {
        pqxx::work txn(*conn);
        auto result = txn.exec_params(
                "SELECT id, username, email, password_hash, role"
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
                "SELECT id, username, email, password_hash, role"
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
std::vector<std::unique_ptr<User>> User::findAll(){
    auto conn = DatabasePool::getInstance().getConnection();
    try {
        pqxx::work txn(*conn);
        auto result = txn.exec_params(
                "SELECT id, username, email, password_hash, role"
                "FROM users "
        );

        if (result.empty()) {
            return nullptr;
        }

        std::vector<std::unique_ptr<User>> users;

        for(auto row : result){
            auto user = User::create()
                .setUsername(row["username"].as<std::string>())
                .setEmail(row["email"].as<std::string>())
                .setPasswordHash(row["password_hash"].as<std::string>())
                .setRole(row["role"].as<std::string>())
                .build();

            user->id_= row["id"].as<int>();
            users.push_back(user);
        }
       txn.commit();

        return users;

    } catch (const std::exception& e) {
        //TODO: Record err log
        return nullptr;
    }

}

bool User::save(){
    if (id_ != 0){
        return update();
    }
    auto conn = DatabasePool::getInstance().getConnection();

    try {
        pqxx::work txn(*conn);

        auto check = txn.exec_params(
            "SELECT id FROM users WHERE username = $1 OR email = $2",
            username_, email_
        );

        if(!check.empty()){
            return false;
        }

        auto result = txn.exec_params(
            "INSERT INTO users (username, email, password_hash, role)"
            "VALUES ($1, $2, $3, $4)"
            "RETURNING id",
            username_, email_, password_hash_, role_
        );

        if(!result.empty()){
            id_=result[0]["id"].as<int>();
            txn.commit();
            return true;
        }
        } catch (const std::exception& e) {
        //TODO: err log
        return false;
    }
}

bool User::update(){
    if(id_ ==0){
        return false;
    }

    auto conn = DatabasePool::getInstance().getConnection();

    try {
        pqxx::work txn(*conn);
        
        auto check = txn.exec_params(
            "SELECT id FROM users WHERE (username = $1 OR email = $2) AND id != $3",
            username_, email_, id_
        );

        if(!check.empty()){
            return false; //Username or email conflict
        }

        auto result = txn.exec_params(
            "UPDATE users"
            "SET username = $1, email = $2, password_hash = $3, role = $4"
            "WHERE id = $5",
            username_, email_, password_hash_, role_, id_
        );
        
        txn.commit();
        return result.affected_rows() > 0;
   } catch (const std::exception& e) {
        //TODO
        return false; 
    }
}
bool User::remove(){
    if(id_ == 0){
        return false;
    }

    auto conn = DatabasePool::getInstance().getConnection();

    try {
        pqxx::work txn(*conn);

        //Check if borrow rec exists.
        auto check = txn.exec_params(
            "SELECT id FROM borrowing_records WHERE user_id = $1 AND return_date IS NULL",
            id_
        );

        if(!check.empty()){
            return false;//user has unreturned book cannot del
        }

        auto result = txn.exec_params(
            "DELETE FROM users WHERE id = $1",
            id_
        );

        txn.commit();
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        return false;
    }
}
