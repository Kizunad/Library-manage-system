// src/service/borrowing_service.cpp

#include "services/borrowing_service.hpp"
#include "models/borrowing_record.hpp"
#include <chrono>
#include <ctime>
#include <exception>
#include <iostream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

std::unique_ptr<BorrowingRecord> BorrowingService::borrowBook(
    int user_id,
    int book_id,
    const std::string& borrow_date,
    const std::string& return_date
){
    try {
        auto user = User::findById(user_id);
        if (user == nullptr) {
            return nullptr;
        }

        auto book = Book::findById(book_id);
        if (book == nullptr) {
            return nullptr;
        }

        if (!validateBorrowLimit(user_id)) {
            return nullptr;
        }

        if (!validateOverdue(user_id)) {
            return nullptr;
        }

        if (!book->borrow()) {
            return nullptr;
        }

        std::string borrow_date_str = borrow_date.empty() ? getCurrentDate() : borrow_date;
        std::string due_date_str = return_date.empty() ? calculateDueDate(borrow_date_str) : return_date;

        auto record = BorrowingRecord::create()
            .setUserId(user_id)
            .setBookId(book_id)
            .setBorrowDate(borrow_date_str)
            .setDueDate(due_date_str)
            .setStatus("borrowed")
            .build();

        if (record->save()) {
            return record;
        }

        book->return_book();
        return nullptr;
    } catch (const std::exception& e) {
        std::cerr << "Error in BorrowingService::borrowBook(): " << e.what() << std::endl;
        return nullptr;
    }
}

bool BorrowingService::returnBook(
    int user_id,
    int book_id,
    const std::string& return_date)
{
    try {
        auto records = BorrowingRecord::findByUserId(user_id);
        for (const auto& record : records){
            if (record->getBookId() == book_id && record->getReturnDate().empty()){
                auto book = Book::findById(book_id);
                if (!book || !book->return_book()) {
                    return false;
                }

                return record->return_book(return_date.empty() ? getCurrentDate() : return_date);
            }
        }
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error in BorrowingService::returnBook(): " << e.what() << std::endl;
        return false;
    }
}

bool BorrowingService::renewBook(int user_id, int book_id){
    try {
            if (!validateOverdue(user_id)) {
                return false;
            }

            auto records = BorrowingRecord::findByUserId(user_id);
            for (const auto& record : records) {
                if (record->getBookId() == book_id && record->getReturnDate().empty()) {
                    return record->renew();
                }
            }
            return false;
    } catch (const std::exception& e) {
        std::cerr << "Error in BorrowingService::renewBook(): " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::unique_ptr<BorrowingRecord>> getUserBorrowings(int user_id, bool include_returned){
    try {
        auto records = BorrowingRecord::findByUserId(user_id);
        if (include_returned) {
            return records;
        }

        std::vector<std::unique_ptr<BorrowingRecord>> active_records;
        for (const auto& record : records) {
            if (record->getReturnDate().empty()) {
                active_records.push_back(std::move(record));
            }
        }
        return active_records;
    }
    catch (const std::exception& e) {
        std::cerr << "Error in BorrowingService::getUserBorrowings(): " << e.what() << std::endl;
        return {};
    }
}

std::vector<std::unique_ptr<BorrowingRecord>> BorrowingService::getBookBorrowings (int book_id, bool include_returned){
    try {
        auto records = BorrowingRecord::findByBookId(book_id);
        if (include_returned) {
            return records;
        }

        std::vector<std::unique_ptr<BorrowingRecord>> active_records;
        for (const auto& record : records) {
            if (record->getReturnDate().empty()) {
                active_records.push_back(std::move(record));
            }
        }
        return active_records;
    }catch (const std::exception& e) {
        std::cerr << "Error in BorrowingService::getBookBorrowings(): " << e.what() << std::endl;
        return {};
    }
}

std::vector<std::unique_ptr<BorrowingRecord>> BorrowingService::getOverdueBooks(){
    try {
        return BorrowingRecord::findOverdue();
    } catch (const std::exception& e) {
        std::cerr << "Error in BorrowingService::getOverdueBooks(): " << e.what() << std::endl;
        return {};
    }
}

int BorrowingService::getUserCurrentBorrowCount(int user_id) const{
    try {
        return BorrowingRecord::countActiveByUserId(user_id);
    } catch (const std::exception& e) {
        std::cerr << "Error in BorrowingService::getUserCurrentBorrowCount(): " << e.what() << std::endl;
        return -1;
    }
}

int BorrowingService::getUserOverdueCount(int user_id) const{
    try {
        return BorrowingRecord::countOverdueByUserId(user_id);
    }catch(const std::exception& e) {
        std::cerr << "Error in BorrowingService::getUserOverdueCount(): " << e.what() << std::endl;
        return -1;
    }
}

std::string BorrowingService::getCurrentDate() const {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d");
    return ss.str();
}


std::string BorrowingService::calculateDueDate(const std::string& borrow_date) const {
    std::tm borrow_date_tm = {};
    std::istringstream ss(borrow_date);
    ss >> std::get_time(&borrow_date_tm, "%Y-%m-%d");
    borrow_date_tm.tm_mday += MAX_BORROW_TIME;
    std::stringstream due_date_ss;
    due_date_ss << std::put_time(&borrow_date_tm, "%Y-%m-%d");
    return due_date_ss.str();
}

bool BorrowingService::validateBorrowLimit(int user_id) const {
    return getUserCurrentBorrowCount(user_id) < 3;
}

bool BorrowingService::validateOverdue(int user_id) const {
    return getUserOverdueCount(user_id) == 0;
}
