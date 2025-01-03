// src/controllers/borrow_controller.cpp

#include "controllers/borrowing_controller.hpp"

nlohmann::json BorrowingController::handleBorrowBook(const nlohmann::json& request){
    try{
         if (!request.contains("user_id") || !request.contains("book_id")) {
            return createErrorResponse("Missing required fields");
        }

        int user_id = request["user_id"];
        int book_id = request["book_id"];

        std::string borrow_date = request.contains("borrow_date") ? request["borrow_date"] : "";
        std::string due_date = request.contains("due_date") ? request["due_date"] : "";

        auto record = borrowingService_.borrowBook(user_id, book_id, borrow_date, due_date);

        if (record == nullptr) {
            return createErrorResponse("Failed to borrow book");
        }

        return createSuccessResponse("Book borrowed successfully", borrowingRecordToJson(record.get()));
        } catch (const std::exception& e) {
            return createErrorResponse(std::string("Error while borrowing_book: ")+e.what());
        }

}

nlohmann::json BorrowingController::handleReturnBook(const nlohmann::json& request){
    try{
        if (!request.contains("book_id") || !request.contains("user_id")) {
            return createErrorResponse("Missing required fields");
        }

        int user_id = request["user_id"];
        int book_id = request["book_id"];
        std::string return_date = request.contains("return_date") ? request["return_date"] : "";
        if(!borrowingService_.returnBook(user_id, book_id, return_date)){
            return createErrorResponse("Failed to return book");
        }
    } catch (const std::exception& e) {
        return createErrorResponse(std::string("Error while returning_book: ")+e.what());
    }
}
nlohmann::json BorrowingController::handleRenewBook(const nlohmann::json& request){
    try{
        if (!request.contains("book_id") || !request.contains("user_id")) {
            return createErrorResponse("Missing required fields");
        }

        int user_id = request["user_id"];
        int book_id = request["book_id"];

        if(!borrowingService_.renewBook(
            user_id,
            book_id)){
            return createErrorResponse("Failed to renew book");
        }
    } catch (const std::exception& e) {
        return createErrorResponse(std::string("Error while renewing_book: ")+e.what());
    }
}

nlohmann::json BorrowingController::handleGetUserBorrowings(const std::string& user_id, const std::string& include_returned){
    try{
        int user_id_int= std::stoi(user_id);
        bool include_returned_bool = include_returned == "true";

        auto records = borrowingService_.getUserBorrowings(user_id_int, include_returned_bool);

        nlohmann::json response = {
            {"success", true},
            {"borrowings", nlohmann::json::array()}
        };

        for (const auto& record : records) {
            response["borrowings"].push_back(borrowingRecordToJson(record.get()));
        }

        return response;
    }
    catch (const std::exception &e)
    {
        return createErrorResponse(std::string("Error while getting user borrowings: ") + e.what());
    }
}
nlohmann::json BorrowingController::handleGetBookBorrowings(const std::string& book_id, const std::string& include_returned){
    try{
        int book_id_int = std::stoi(book_id);
        bool include_returned_bool = include_returned == "true";

        auto records = borrowingService_.getBookBorrowings(book_id_int, include_returned_bool);

        nlohmann::json response = {
            {"success", true},
            {"borrowings", nlohmann::json::array()}
        };

        for (const auto& record : records) {
            response["borrowings"].push_back(borrowingRecordToJson(record.get()));
        }

        return response;
    }
    catch (const std::exception &e)
    {
        return createErrorResponse(std::string("Error while getting book borrowings: ") + e.what());
    }
}
nlohmann::json BorrowingController::handleGetOverdueBooks(){
    try{
        auto records = borrowingService_.getOverdueBooks();

        nlohmann::json response = {
            {"success", true},
            {"borrowings", nlohmann::json::array()}
        };

        for (const auto& record : records) {
            response["borrowings"].push_back(borrowingRecordToJson(record.get()));
        }

        return response;
    }
    catch (const std::exception &e)
    {
        return createErrorResponse(std::string("Error while getting overdue books: ") + e.what());
    }
}

nlohmann::json BorrowingController::handleGetUserBorrowStatus(const std::string& user_id){
    try
    {
        int user_id_int = std::stoi(user_id);

        int current_borrowed = borrowingService_.getUserCurrentBorrowCount(user_id_int);
        int overdue_count = borrowingService_.getUserOverdueCount(user_id_int);

        nlohmann::json status = {
            {"current_borrowed", current_borrowed},
            {"overdue_count", overdue_count}
        };
        return createSuccessResponse("User borrow status retrieved successfully", status);
    }
    catch(const std::exception& e)
    {
        return createErrorResponse(std::string("Error while getting user borrow status: ") + e.what());
    }

}

nlohmann::json createSuccessResponse(const std::string& message, const nlohmann::json& data = nullptr){
nlohmann::json response = {
    {"success", true},
    {"message", message}
};

if (data != nullptr) {
    response["data"] = data;
}

return response;
}

nlohmann::json createErrorResponse(const std::string& message){
    return {
        {"success", false},
        {"error", message}
    };
}

nlohmann::json borrowingRecordToJson(const BorrowingRecord* record){
    if (record == nullptr) {
        return nullptr;
    }

    return {
        {"id", record->getId()},
        {"user_id", record->getUserId()},
        {"book_id", record->getBookId()},
        {"borrow_date", record->getBorrowDate()},
        {"due_date", record->getDueDate()},
        {"return_date", record->getReturnDate()},
        {"status", record->getStatus()}
    };
}
