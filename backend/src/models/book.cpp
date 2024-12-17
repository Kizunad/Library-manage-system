// src/models/book.cpp

#include "models/book.hpp"
#include "utils/database_pool.hpp"
#include <cstddef>
#include <iostream>
#include <exception>


std::unique_ptr<Book> Book::findById(int id)
{
    auto conn = DatabasePool::getInstance().getConnection();
    try
    {
        pqxx::work txn(*conn);
        
        auto result = txn.exec_params(
                "SELECT * FROM books WHERE id = $1",
                id
            );

        if (result.empty()){
            return nullptr;
        }

        const auto& row = result[0];
        auto book = Book::create()
            .setIsbn(row["isbn"].as<std::string>())
            .setTitle(row["title"].as<std::string>())
            .setAuthor(row["author"].as<std::string>())
            .setPublisher(row["publisher"].as<std::string>())
            .setPublishDate(row["publish_date"].as<std::string>())
            .setCategory(row["category"].as<std::string>())
            .setTotalCopies(row["total_copies"].as<int>())
            .build();
        
        book->id_ = id;
        book->available_copies_ = row["available_copies"].as<int>();

        txn.commit();
        return book;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error in findById(): "<< e.what() <<std::endl;
        //TODO
        return nullptr;
    }
}
std::unique_ptr<Book> Book::findByIsbn(const std::string &isbn){
auto conn = DatabasePool::getInstance().getConnection();
    try
    {
        pqxx::work txn(*conn);
        
        auto result = txn.exec_params(
                "SELECT * FROM books WHERE isbn = $1",
                isbn
            );

        if (result.empty()){
            return nullptr;
        }

        const auto& row = result[0];
        auto book = Book::create()
            .setIsbn(row["isbn"].as<std::string>())
            .setTitle(row["title"].as<std::string>())
            .setAuthor(row["author"].as<std::string>())
            .setPublisher(row["publisher"].as<std::string>())
            .setPublishDate(row["publish_date"].as<std::string>())
            .setCategory(row["category"].as<std::string>())
            .setTotalCopies(row["total_copies"].as<int>())
            .build();
        
        book->id_ = row["id"].as<int>();
        book->available_copies_ = row["available_copies"].as<int>();

        txn.commit();
        return book;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error in findById(): "<< e.what() <<std::endl;
        //TODO
        return nullptr;
    }
   
}
std::vector<std::unique_ptr<Book>> Book::findAll(int page, int pagesize){
    auto conn = DatabasePool::getInstance().getConnection();
    std::vector<std::unique_ptr<Book>> books;
    try {
        pqxx::work txn(*conn);
        auto result = txn.exec_params(
                "SELECT *"
                "FROM books "
        );

        if (result.empty()) {
            return books;
        }

       
        for(auto row : result){
            auto book = Book::create()
                .setIsbn(row["isbn"].as<std::string>())
                .setTitle(row["title"].as<std::string>())
                .setAuthor(row["author"].as<std::string>())
                .setPublisher(row["publisher"].as<std::string>())
                .setPublishDate(row["publish_date"].as<std::string>())
                .setCategory(row["category"].as<std::string>())
                .setTotalCopies(row["total_copies"].as<int>())
                .build();
        
            book->id_ = row["id"].as<int>();
            book->available_copies_ = row["available_copies"].as<int>();

            books.push_back(book);
        }
       txn.commit();

        return books;

    } catch (const std::exception& e) {
       std::cerr << "Error in Book::findAll(): " << e.what() <<std::endl;
        return books;
    }
}
   

std::vector<std::unique_ptr<Book>> search(const std::string &keyword);

bool Book::save() {
    auto conn = DatabasePool::getInstance().getConnection();
    try{
        if (id_ != 0) {
        return update();
    }

    auto conn = DatabasePool::getInstance().getConnection();
    try {
        pqxx::work txn(*conn);
        
        // 检查ISBN是否已存在
        auto check = txn.exec_params(
            "SELECT id FROM books WHERE isbn = $1",
            isbn_
        );
        
        if (!check.empty()) {
            return false; // ISBN已存在
        }

        auto result = txn.exec_params(
            "INSERT INTO books (isbn, title, author, publisher, publish_date, "
            "category, total_copies, available_copies) "
            "VALUES ($1, $2, $3, $4, $5, $6, $7, $8) RETURNING id",
            isbn_, title_, author_, publisher_, publish_date_, 
            category_, total_copies_, available_copies_
        );
        
        if (!result.empty()) {
            id_ = result[0]["id"].as<int>();
            txn.commit();
            return true;
        }
        return false;
    } catch (const std::exception& e){
        std::cerr << "Error in Book::Save(): " << e.what() <<std::endl;
        return false;
    }
}
bool update();
bool remove();

bool borrow();
bool return_book();