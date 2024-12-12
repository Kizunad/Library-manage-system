// include/models/user.hpp

#pragma once
#include <stdexcept>
#include <string>
#include <memory>
#include <utility>
#include <vector>
#include <pqxx/pqxx>

class User {
    public:
        class UserBuilder {
            public:

            UserBuilder() : user_(new User()) {}
            UserBuilder& setUsername(const std::string& username){
                user_->username_ = username;
                return *this;
            }
            UserBuilder& setEmail(const std::string& email){
                user_->email_ = email;
                return *this;
            }
            UserBuilder& setPasswordHash(const std::string& hash){
                user_->password_hash_ = hash;
                return *this;
            }
            UserBuilder& setRole(const std::string& role){
                user_->role_ = role;
                return *this;
            }

            std::unique_ptr<User> build(){
                if (user_->username_.empty()      ||
                    user_->email_.empty()         ||
                    user_->password_hash_.empty() ||
                    user_->role_.empty()) {
                        throw std::invalid_argument("Required field cannot be empty");
                }
                return std::move(user_);
            }

            private:
            std::unique_ptr<User> user_;
        };

        static UserBuilder create() {
            return {};
        }

        static std::unique_ptr<User> findById(int id);
        static std::unique_ptr<User> findByUsername(const std::string& username);
        static std::vector<std::unique_ptr<User>> findAll();

        bool save();
        bool update();
        bool remove();
    private:
        int id_{};
        std::string username_;
        std::string email_;
        std::string password_hash_;
        std::string role_;

        User() = default;
        friend class UserBuilder;
    };
