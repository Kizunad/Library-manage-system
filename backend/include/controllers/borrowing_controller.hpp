// include/controllers/borrowing_controller.hpp

#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include "models/borrowing_record.hpp"
#include "services/borrowing_service.hpp"

class BorrowingController {
public:
    static BorrowingController& getInstance(){
        static BorrowingController instance;
        return instance;
    }

    nlohmann::json handleBorrowBook(const nlohmann::json& request);
    nlohmann::json handleReturnBook(const nlohmann::json& request);
    nlohmann::json handleRenewBook(const nlohmann::json& request);

    nlohmann::json handleGetUserBorrowings(const std::string& user_id, const std::string& include_returned);
    nlohmann::json handleGetBookBorrowings(const std::string& book_id, const std::string& include_returned);
    nlohmann::json handleGetOverdueBooks();

    nlohmann::json handleGetUserBorrowStatus(const std::string& user_id);
private:
    BorrowingController() = default;
    BorrowingController(const BorrowingController&) = delete;
    BorrowingController& operator=(const BorrowingController&) = delete;

    BorrowingService& borrowingService_ = BorrowingService::getInstance();

    nlohmann::json createSuccessResponse(const std::string& message, const nlohmann::json& data = nullptr);
    nlohmann::json createErrorResponse(const std::string& message);
    nlohmann::json borrowingRecordToJson(const BorrowingRecord* record);
};
