// include/services/borrowing_service.hpp
#include <memory.h>
#include <memory>
#include <string>
#include <vector>
#include "models/borrowing_record.hpp"
#include "models/book.hpp"
#include "models/user.hpp"
class BorrowingService{
public:
    static BorrowingService& getInstance(){
        static BorrowingService instance;
        return instance;
    }

    BorrowingService(const BorrowingService&) = delete;
    BorrowingService& operator=(const BorrowingService&) = delete;

    [[nodiscard]] std::unique_ptr<BorrowingRecord> borrowBook(int user_id, int book_id,
     const std::string& borrow_date = "", const std::string& return_date = "");
    [[nodiscard]] bool returnBook(int user_id, int book_id,
    const std::string& return_date = "");

    [[nodiscard]] bool renewBook(int user_id, int book_id);

    [[nodiscard]] std::vector<std::unique_ptr<BorrowingRecord>>
    getUserBorrowings(int user_id, bool include_returned = false);

    [[nodiscard]] std::vector<std::unique_ptr<BorrowingRecord>>
    getBookBorrowings(int book_id, bool include_returned = false);

    [[nodiscard]] std::vector<std::unique_ptr<BorrowingRecord>>
    getOverdueBooks();

    [[nodiscard]] int getUserCurrentBorrowCount(int user_id);
    [[nodiscard]] int getUserOverdueCount(int user_id);

private:
    BorrowingService() = default;
    [[nodiscard]] std::string getCurrentDate() const;
    [[nodiscard]] std::string calculateDueDate(const std::string& borrow_date) const;
    [[nodiscard]] bool validateBorrowLimit(int user_id) const;
    [[nodiscard]] bool validateOverdue(int user_id) const;
};
