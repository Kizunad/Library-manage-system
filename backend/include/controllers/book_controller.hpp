// include/controller/book_controller.hpp

#include <nlohmann/json_fwd.hpp>
#include <string>
#include <nlohmann/json.hpp>
#include "models/book.hpp"
#include "services/book_service.hpp"

class BookController {
public:
    static BookController& getInstance(){
        static BookController instance;
        return instance;
    }
    
    nlohmann::json handleGetAllBooks(const std::string& page, const std::string& pageSize);
    nlohmann::json handleGetBook(const std::string& book_id);
    nlohmann::json handleSearchBook(const std::string& keyword);
    nlohmann::json handleAddBook(const nlohmann::json& book_data);
    nlohmann::json handleUpdateBook(const std::string& book_id, const nlohmann::json& book_data);
    nlohmann::json handleDeleteBook(const std::string& book_id);
    nlohmann::json handleBorrowBook(const std::string& user_id, const std::string& book_id);
    nlohmann::json handleReturnBook(const std::string& user_id, const std::string& book_id);
    nlohmann::json handleGetBorrowedBooks(const std::string& user_id);
private:
    BookController() = default;
    BookController(const BookController&) = delete;
    BookController& operator=(const BookController&) = delete;
    
    BookService& bookService_ = BookService::getInstance();
};