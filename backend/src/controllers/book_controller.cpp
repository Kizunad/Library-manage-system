// src/controllers/book_controller.cpp

#include "controllers/book_controller.hpp"
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