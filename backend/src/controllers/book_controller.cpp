// src/controllers/book_controller.cpp

#include "controllers/book_controller.hpp"
#include "models/book.hpp"
#include <exception>
#include <string>

nlohmann::json BookController::handleGetAllBooks(
    const std::string& page,
    const std::string& pageSize
){
    try {
            int page_ = std::stoi(page);
            int pageSize_ = std::stoi(pageSize);
            auto books = bookService_.getAllBooks(page_, pageSize_);
            int total = bookService_.getTotalBooks();

            nlohmann::json response = {
                {"success", true},
                {"total", total},
                {"page", page_},
                {"pageSize", pageSize_},
                {"books", nlohmann::json::array()}
            };

            for (const auto& book : books) {
            response["books"].push_back({
                {"id", book->getId()},
                {"isbn", book->getIsbn()},
                {"title", book->getTitle()},
                {"author", book->getAuthor()},
                {"publisher", book->getPublisher()},
                {"publishDate", book->getPublishDate()},
                {"category", book->getCategory()},
                {"totalCopies", book->getTotalCopies()},
                {"availableCopies", book->getAvailableCopies()}
            });}

            return response;

    } catch (const std::exception& e) {
        //TODO Logging
        return {
            {"success", false},
            {"error", e.what()}
        };
    }
}

nlohmann::json BookController::handleAddBook(const nlohmann::json& book_data) {
    try {
        auto book = bookService_.addBook(
            book_data["isbn"],
            book_data["title"],
            book_data["author"],
            book_data["publisher"],
            book_data["publishDate"],
            book_data["category"],
            book_data["totalCopies"]
        );

        if (book) {
            return {
                {"success", true},
                {"book", {
                    {"id", book->getId()},
                    {"isbn", book->getIsbn()},
                    {"title", book->getTitle()}
                }}
            };
        }

        return {
            {"success",false},
            {"error","Failed to add Book."}
        };
    } catch (const std::exception& e) {
        return {
            {"success", false},
            {"error", e.what()}
        };
    }
}
