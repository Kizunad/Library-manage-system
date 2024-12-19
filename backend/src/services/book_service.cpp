//  src/services/book_services.cpp
#include "services/book_service.hpp"
#include "models/book.hpp"
#include "models/user.hpp"
#include "utils/database_pool.hpp"
#include <exception>
#include <memory>
#include <iostream>

std::unique_ptr<Book> BookService::addBook(
    const std::string&  isbn,
    const std::string&  title,
    const std::string&  author,
    const std::string&  publisher,
    const std::string&  publish_date,
    const std::string&  category,
    int                 total_copies
){
    try {
        auto book = Book::create()
            .setIsbn(isbn)
            .setTitle(title)
            .setAuthor(author)
            .setPublisher(publisher)
            .setPublishDate(publish_date)
            .setCategory(category)
            .setTotalCopies(total_copies)
            .build();

            if (book->save()) {
                return book;
            }

            return nullptr;
    }
    catch (const std::exception& e){
        std::cerr << "Error in BookService::addBook() :" 
                  << e.what() 
                  << std::endl;
        return nullptr;
    }
}

bool BookService::borrowBook(
    int user_id,
    int book_id
){
    try{
        auto user = User::findById(user_id);
            if(user == nullptr){
            //TODO: logging
            return false;
        }
    
        auto book = Book::findById(book_id);
        if(book == nullptr){
            //TODO: logging
            return false;
        }
    
        auto conn = DatabasePool::getInstance().getConnection();
        pqxx::work txn(*conn);
    
        //TODO: 
        //by checking the BorrowingRecords db,
        //if user borrowed this book return false.
    
        if (!book->borrow()){
            //TODO: Loggin
            return false;
        }
    
        //TODO:
        //Create record in BorrowingRecord.db
    
        txn.commit();
    
        return true;
        } catch (const std::exception& e){
            //TODO: Logging
        return false;
    }
}
