// include/services/book_service.hpp

#include <memory.h>
#include <memory>
#include <string>
#include <vector>
#include <cstring>
#include "models/user.hpp"
#include "models/book.hpp"

#define PAGESIZE 10
class BookService {
public:
    BookService(const BookService&) = delete;
    BookService& operator=(const BookService&) = delete;

    static BookService& getInstance(){
        static BookService instance;
        return instance;
    }

    [[nodiscard]] std::unique_ptr<Book> addBook(
        const std::string& isbn,
        const std::string& title,
        const std::string& author,
        const std::string& publisher,
        const std::string& publish_date,
        const std::string& category,
        int total_copies
    );

    [[nodiscard]] bool updateBook(
        int book_id,
        const std::string& isbn,
        const std::string& title,
        const std::string& author,
        const std::string& publisher,
        const std::string& publish_date,
        const std::string& category,
        int total_copies
    );

    // Search in books
    [[nodiscard]] bool removeBook(int book_id);
    [[nodiscard]] std::unique_ptr<Book> getBookById(int book_id);
    [[nodiscard]] std::unique_ptr<Book> getBookByIsdn(const std::string &isdn);
    [[nodiscard]] std::vector<std::unique_ptr<Book>> SearchBooks(const std::string& keyword);
    [[nodiscard]] std::vector<std::unique_ptr<Book>> getAllBooks(int page = 1, int pagesize = PAGESIZE);
    [[nodiscard]] int getTotalBooks();


    //Borrow Return
    [[nodiscard]] bool borrowBook(int user_id, int book_id);
    [[nodiscard]] bool returnBook(int user_id, int book_id);
    [[nodiscard]] std::vector<std::unique_ptr<Book>> getBorrowedBooks(int user_id);

private:
    BookService() = default;
};
