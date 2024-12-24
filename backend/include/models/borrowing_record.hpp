// include/models/borrowing_record.hpp

#pragma once
#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <pqxx/pqxx>

class BorrowingRecord {
public:
    // 借阅状态枚举
    enum class Status {
        BORROWED,   // 已借出
        RETURNED,   // 已归还
        OVERDUE,    // 已逾期
        RENEWED     // 已续借
    };

    class BorrowingRecordBuilder {
    public:
        BorrowingRecordBuilder() : record_(new BorrowingRecord()) {}

        BorrowingRecordBuilder& setUserId(int user_id) {
            record_->user_id_ = user_id;
            return *this;
        }

        BorrowingRecordBuilder& setBookId(int book_id) {
            record_->book_id_ = book_id;
            return *this;
        }

        BorrowingRecordBuilder& setBorrowDate(const std::string& borrow_date) {
            record_->borrow_date_ = borrow_date;
            return *this;
        }

        BorrowingRecordBuilder& setDueDate(const std::string& due_date) {
            record_->due_date_ = due_date;
            return *this;
        }

        BorrowingRecordBuilder& setReturnDate(const std::string& return_date) {
            record_->return_date_ = return_date;
            return *this;
        }

        BorrowingRecordBuilder& setStatus(const std::string& status) {
            record_->status_ = status;
            return *this;
        }

        std::unique_ptr<BorrowingRecord> build() {
            if (record_->user_id_ <= 0 || record_->book_id_ <= 0) {
                throw std::invalid_argument("User ID and Book ID must be valid!");
            }
            if (record_->borrow_date_.empty() || record_->due_date_.empty()) {
                throw std::invalid_argument("Borrow date and due date must be set!");
            }
            return std::move(record_);
        }

    private:
        std::unique_ptr<BorrowingRecord> record_;
    };

    static BorrowingRecordBuilder create() {
        return {};
    }

    // 查询方法
    static std::unique_ptr<BorrowingRecord> findById(int record_id);
    static std::vector<std::unique_ptr<BorrowingRecord>> findByUserId(int user_id);
    static std::vector<std::unique_ptr<BorrowingRecord>> findByBookId(int book_id);
    static std::vector<std::unique_ptr<BorrowingRecord>> findOverdue();
    static std::vector<std::unique_ptr<BorrowingRecord>> findAll();

    // 统计方法
    static int countActiveByUserId(int user_id);  // 获取用户当前借阅数量
    static int countOverdueByUserId(int user_id); // 获取用户逾期数量

    // CRUD操作
    bool save();
    bool update();
    bool remove();

    // 业务操作
    bool return_book(const std::string& return_date);
    bool renew(); // 续借功能
    [[nodiscard]] bool isOverdue() const; // 检查是否逾期

    // Getters
    [[nodiscard]] int getId() const { return id_; }
    [[nodiscard]] int getUserId() const { return user_id_; }
    [[nodiscard]] int getBookId() const { return book_id_; }
    [[nodiscard]] const std::string& getBorrowDate() const { return borrow_date_; }
    [[nodiscard]] const std::string& getDueDate() const { return due_date_; }
    [[nodiscard]] const std::string& getReturnDate() const { return return_date_; }
    [[nodiscard]] const std::string& getStatus() const { return status_; }

private:
    int id_{0};
    int user_id_{0};
    int book_id_{0};
    std::string borrow_date_;
    std::string due_date_;
    std::string return_date_;
    std::string status_{"borrowed"};

    BorrowingRecord() = default;
    friend class BorrowingRecordBuilder;
};
