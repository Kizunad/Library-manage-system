// src/models/borrowing_record.cpp

#include "models/borrowing_record.hpp"
#include "utils/database_pool.hpp"
#include <exception>
#include <iostream>
#include <ctime>
#include <memory>
#include <sys/types.h>
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

bool BorrowingRecord::save(){
    auto conn = DatabasePool::getInstance().getConnection();

    try {
        pqxx::work txn(*conn);

        auto check = txn.exec_params(
            "SELECT id FROM borrowing_records "
            "WHERE user_id = $1 AND book_id = $2 AND return_date IS NULL",
            user_id_, book_id_
        );

        if (!check.empty()){
            return false;
        }

        auto result = txn.exec_params(
            "INSERT INTO borrowing_records (user_id, book_id, borrow_date, due_date, status)"
            "VALUES ($1, $2, $3, $4, $5) RETURNING id",
            user_id_, book_id_, borrow_date_, due_date_, status_
        );

        if (!result.empty()) {
            id_ = result[0]["id"].as<int>();
            txn.commit();
            return true;
        }
        return false;

    } catch (const std::exception& e) {
        std::cerr << "Error in BorrowingRecord::save(): " << e.what() << std::endl;
        return false;
    }
}

bool BorrowingRecord::update(){
    if (id_ == 0) {
        return false;
    }

    auto conn = DatabasePool::getInstance().getConnection();

    try {
        pqxx::work txn(*conn);

        auto result = txn.exec_params(
            "UPDATE borrowing_records "
            "SET user_id = $1, book_id = $2, borrow_date = $3, due_date = $4, status = $5 "
            "WHERE id = $6",
            user_id_, book_id_, borrow_date_, due_date_, status_, id_
        );

        txn.commit();
        return result.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Error in BorrowingRecord::update(): " << e.what() << std::endl;
        return false;
    }

}

bool BorrowingRecord::return_book(const std::string& return_date){
    if (id_ == 0 || !return_date_.empty()) {
        return false;
    }
    auto conn = DatabasePool::getInstance().getConnection();

    try {
        pqxx::work txn(*conn);

        return_date_ = return_date;
        status_ = "returned";

        auto result = txn.exec_params(
            "UPDATE borrowing_records "
            "SET return_date = $1, status = $2 "
            "WHERE id = $3",
            return_date_, status_, id_
        );

        txn.commit();
        return result.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Error in BorrowingRecord::return_book(): " << e.what() << std::endl;
        return false;
    }
}

bool BorrowingRecord::renew(){
    if (id_ == 0 || !return_date_.empty() || status_ == "renewed") {
        return false;
    }

    auto conn = DatabasePool::getInstance().getConnection();

    try {
        pqxx::work txn(*conn);

        auto result = txn.exec_params(
            "UPDATE borrowing_records SET due_date = due_date + INTERVAL '14 days' status = 'renewed' WHERE id = $1 AND reutrn_date IS NULL",
            id_
        );

        if (result.affected_rows() > 0) {
           status_ = "renewed";

           auto updated = txn.exec_params(
            "SELECT due_date FROM borrowing_records WHERE id = $1",id_
           );
           due_date_ = updated[0]["due_date"].as<std::string>();

           txn.commit();
           return true;
        }
        return false;
    }catch (const std::exception& e) {
        std::cerr << "Error in BorrowingRecord::renew(): " << e.what() << std::endl;
        return false;
    }
}

bool BorrowingRecord::isOverdue() const {
    if (return_date_.empty()) {
        return false;
    }

    try {
        auto conn = DatabasePool::getInstance().getConnection();

        pqxx::work txn(*conn);

        auto result = txn.exec_params(
            "SELECT due_date < CURRENT_TIMESTAMP as is_overdue FROM borrowing_records WHERE id = $1",
            id_
        );

        if (!result.empty()){
           return result[0]["is_overdue"].as<bool>();
        }
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error in BorrowingRecord::isOverdue(): " << e.what() << std::endl;
        return false;
    }
}
