// src/models/book.cpp

#include "models/book.hpp"
#include "utils/database_pool.hpp"
#include <iostream>
#include <exception>
#include <vector>


std::unique_ptr<Book> Book::findById(int book_id)
{
    auto conn = DatabasePool::getInstance().getConnection();
    try
    {
        pqxx::work txn(*conn);

        auto result = txn.exec_params(
                "SELECT * FROM books WHERE id = $1",
                book_id
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
std::vector<std::unique_ptr<Book>> Book::search(const std::string& keyword){
    std::vector<std::unique_ptr<Book>> books;
    auto conn = DatabasePool::getInstance().getConnection();

    try {
        pqxx::work txn(*conn);

        auto res = txn.exec_params(
            "SELECT * FROM books"
            "WHERE title ILIKE $1"
            "   OR author ILIKE $1"
            "   OR isbn ILIKE $1"
            "   OR publisher ILIKE $1"
            "   OR category ILIKE $1"
            "ORDER BY "
            "   CASE "
            "       WHEN title ILIKE $1 THEN 1 "  // 标题匹配优先
            "       WHEN author ILIKE $1 THEN 2 "  // 作者匹配次之
            "       WHEN isbn = $2 THEN 3 "        // ISBN精确匹配
            "       ELSE 4 "
            "   END, "
            "   title ASC "
            "LIMIT 100",  // 限制返回数量
            "%" + keyword + "%",  // 用于模糊匹配的参数
            keyword               // 用于精确匹配的参数
        );

         for (const auto& row : res) {
            try {
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
                books.push_back(std::move(book));
            } catch (const std::exception& e) {
                // 如果单个图书对象创建失败，记录错误但继续处理其他结果
                std::cerr << "Error creating book object: " << e.what() << std::endl;
                continue;
            }
        }
        return books;
    } catch (const std::exception& e) {
        return books;
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
bool Book::update(){
    if (id_ == 0){
        return false;
    }

    auto conn = DatabasePool::getInstance().getConnection();

    try{
        pqxx::work txn(*conn);

        auto check = txn.exec_params(
            "SELECT id FROM books WHERE isbn = $1 AND id != $2",
            isbn_, id_
        );

        if (!check.empty()){
            return false;
        }

        auto result = txn.exec_params(
            "UPDATE books SET isbn = $1, title = $2, author = $3,"
            "publisher = $4, publish_date = $5, categroy = $6, "
            "total_copies = $7, available_copies = $8 WHERE id = $9",
            isbn_, title_, author_, publisher_, publish_date_,
            category_, total_copies_, available_copies_, id_
        );

        txn.commit();

        return result.affected_rows() > 0;

    } catch (const std::exception& e){
        std::cerr << "Error in Book::Update():" << e.what() << std::endl;
        return false;
    }
}
bool Book::remove(){
    if (id_ == 0) {
        return false;
    }

    auto conn = DatabasePool::getInstance().getConnection();
    try {
        pqxx::work txn(*conn);

        // 检查是否有未归还的借阅记录
        auto check = txn.exec_params(
            "SELECT id FROM borrowing_records "
            "WHERE book_id = $1 AND return_date IS NULL",
            id_
        );

        if (!check.empty()) {
            return false; // 有未归还的记录，不能删除
        }

        auto result = txn.exec_params(
            "DELETE FROM books WHERE id = $1",
            id_
        );

        txn.commit();
        return result.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Error in remove(): " << e.what() << std::endl;
        return false;
    }
}

bool Book::borrow(){
    if (available_copies_ <= 0) {
        return false;
    }

     auto conn = DatabasePool::getInstance().getConnection();
    try {
        pqxx::work txn(*conn);

        available_copies_--;
        auto result = txn.exec_params(
            "UPDATE books SET available_copies = $1 WHERE id = $2",
            available_copies_, id_
        );

        txn.commit();
        return result.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Error in borrow(): " << e.what() << std::endl;
        return false;
    }

}
bool Book::return_book(){
    if (available_copies_ >= total_copies_) {
        return false;
    }

     auto conn = DatabasePool::getInstance().getConnection();
    try {
        pqxx::work txn(*conn);

        available_copies_++;
        auto result = txn.exec_params(
            "UPDATE books SET available_copies = $1 WHERE id = $2",
            available_copies_, id_
        );

        txn.commit();
        return result.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Error in return_book(): " << e.what() << std::endl;
        return false;
    }
}
