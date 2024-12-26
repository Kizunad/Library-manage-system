// src/models/borrowing_record.cpp

#include "models/borrowing_record.hpp"
#include "utils/database_pool.hpp"
#include <algorithm>
#include <exception>
#include <iostream>
#include <chrono>
#include <ctime>
#include <memory>
#include <sstream>
#include <iomanip>
#include <vector>

std::unique_ptr<BorrowingRecord> BorrowingRecord::findById(int id){
    auto conn = DatabasePool::getInstance().getConnection();

    try {
        pqxx::work txn(*conn);

        auto result = txn.exec_params(
            "SELECT * FROM borrowing_records WHERE id = $1",
            id
        );

        if (result.empty()) {
            return nullptr;
        }

        const auto& row = result[0];

        auto record = BorrowingRecord::create()
            .setUserId(row["user_id"].as<int>())
            .setBookId(row["book_id"].as<int>())
            .setBorrowDate(row["borrow_date"].as<std::string>())
            .setReturnDate(row["return_date"].as<std::string>())
            .build();

        if (!row["return_date"].is_null()) {
            record->return_date_ = (row["return_date"].as<std::string>());
        }
        record->id_ = row["id"].as<int>();

        txn.commit();

        return record;

    } catch (const std::exception& e) {
        std::cerr << "Error in BorrowingRecord::findById(): " << e.what() << std::endl;
        return nullptr;
    }
}

std::vector<std::unique_ptr<BorrowingRecord>> BorrowingRecord::findByUserId(int user_id){
    std::vector<std::unique_ptr<BorrowingRecord>> records;
    auto conn = DatabasePool::getInstance().getConnection();

    try {
        pqxx::work txn(*conn);

        auto result = txn.exec_params(
            "SELECT * FROM borrowing_records WHERE user_id = $1 ORDER BY borrow_date DESC",
            user_id
        );

        for (const auto& row : result) {
            auto record = BorrowingRecord::create()
                .setUserId(row["user_id"].as<int>())
                .setBookId(row["book_id"].as<int>())
                .setBorrowDate(row["borrow_date"].as<std::string>())
                .setDueDate(row["due_date"].as<std::string>())
                .setStatus(row["status"].as<std::string>())
                .build();

            if (!row["return_date"].is_null()) {
                record->return_date_ = row["return_date"].as<std::string>();
            }

            record->id_ = row["id"].as<int>();
            records.push_back(std::move(record));
        }

        txn.commit();

    } catch (const std::exception& e) {
        std::cerr << "Error in BorrowingRecord::findByUserId(): " << e.what() << std::endl;
    }
    return records;
}

std::vector<std::unique_ptr<BorrowingRecord>> BorrowingRecord::findByBookId(int book_id) {
    std::vector<std::unique_ptr<BorrowingRecord>> records;
    auto conn = DatabasePool::getInstance().getConnection();
    try {
        pqxx::work txn(*conn);
        auto result = txn.exec_params(
            "SELECT * FROM borrowing_records "
            "WHERE book_id = $1 "
            "ORDER BY borrow_date DESC",
            book_id
        );

        for (const auto& row : result) {
            auto record = BorrowingRecord::create()
                .setUserId(row["user_id"].as<int>())
                .setBookId(row["book_id"].as<int>())
                .setBorrowDate(row["borrow_date"].as<std::string>())
                .setDueDate(row["due_date"].as<std::string>())
                .setStatus(row["status"].as<std::string>())
                .build();

            if (!row["return_date"].is_null()) {
                record->return_date_ = row["return_date"].as<std::string>();
            }

            record->id_ = row["id"].as<int>();
            records.push_back(std::move(record));
        }
        txn.commit();
    } catch (const std::exception& e) {
        std::cerr << "Error in BorrowingRecord::findByBookId(): " << e.what() << std::endl;
    }
    return records;
}

std::vector<std::unique_ptr<BorrowingRecord>> BorrowingRecord::findOverdue(){
    std::vector<std::unique_ptr<BorrowingRecord>> records;
    auto conn = DatabasePool::getInstance().getConnection();

    try {
        pqxx::work txn(*conn);

        auto result = txn.exec_params(
            "SELECT * FROM borrowing_records "
            "WHERE return_date IS NULL AND due_date < CURRENT_TIMESTAMP"
            "AND status = 'borrowed'"
            "ORDER BY borrow_date DESC"
        );

        for (const auto& row : result) {
            auto record = BorrowingRecord::create()
                .setUserId(row["user_id"].as<int>())
                .setBookId(row["book_id"].as<int>())
                .setBorrowDate(row["borrow_date"].as<std::string>())
                .setDueDate(row["due_date"].as<std::string>())
                .setStatus(row["status"].as<std::string>())
                .build();

            if (!row["return_date"].is_null()) {
                record->return_date_ = row["return_date"].as<std::string>();
            }

            record->id_ = row["id"].as<int>();
            records.push_back(std::move(record));
        }

        txn.commit();

    } catch (const std::exception& e) {
        std::cerr << "Error in BorrowingRecord::findOverdue(): " << e.what() << std::endl;
    }
    return records;
}

int BorrowingRecord::countActiveByUserId(int user_id){
    auto conn = DatabasePool::getInstance().getConnection();

    try {
        pqxx::work txn(*conn);

        auto result = txn.exec_params(
            "SELECT COUNT(*) FROM borrowing_records "
            "WHERE user_id = $1 AND return_date IS NULL",
            user_id
        );

        if (result.empty()) {
            return -1;
        }

        return result[0][0].as<int>();

    } catch (const std::exception& e) {
        std::cerr << "Error in BorrowingRecord::countActiveByUserId(): " << e.what() << std::endl;
        return -1;
    }
}

int BorrowingRecord::countOverdueByUserId(int user_id){
    auto conn = DatabasePool::getInstance().getConnection();

    try {
        pqxx::work txn(*conn);

        auto result = txn.exec_params(
            "SELECT COUNT(*) FROM borrowing_records "
            "WHERE user_id = $1 AND return_date IS NULL AND due_date < CURRENT_TIMESTAMP"
        );

        if (result.empty()) {
            return -1;
       }

       return result[0][0].as<int>();
    } catch (const std::exception& e) {
        std::cerr << "Error in BorrowingRecord::countOverdueByUserId(): " << e.what() << std::endl;
        return -1;
    }
}
