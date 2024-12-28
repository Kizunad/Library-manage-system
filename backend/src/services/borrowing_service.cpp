// src/service/borrowing_service.cpp

#include "services/borrowing_service.hpp"
#include "models/borrowing_record.hpp"
#include <chrono>
#include <ctime>
#include <exception>
#include <iostream>
#include <iomanip>
#include <sstream>

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
