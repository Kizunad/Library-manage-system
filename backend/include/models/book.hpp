// include/models/book.hpp

#pragma once
#include <stdexcept>
#include <string>
#include <memory>
#include <vector>
#include <pqxx/pqxx>

class Book{
public:
    class BookBuilder {
        public:
            BookBuilder() : book_(new Book()){}

            BookBuilder& setIsbn(const std::string& isbn){
                book_->isbn_ = isbn;
                return *this;
            }

            BookBuilder& setAuthor(const std::string& author){
                book_->author_ = author;
                return *this;
            }

            BookBuilder& setTitle(const std::string& title){
                book_->title_ = title;
                return *this;
            }
             BookBuilder& setPublisher(const std::string& publisher){
                book_->publisher_ = publisher;
                return *this;
            }

            BookBuilder& setPublishDate(const std::string& publish_date){
                book_->publish_date_ = publish_date;
                return *this;
            }

            BookBuilder& setCategory(const std::string& category){
                book_->category_ = category;
                return *this;
            }

            BookBuilder& setTotalCopies(const int& copies){
                book_->total_copies_ = copies;
                return *this;
            }

            std::unique_ptr<Book> build(){
                if(
                    book_->isbn_.empty()    ||
                    book_->title_.empty()   ||
                    book_->author_.empty()
                ){
                    throw std::invalid_argument("ISBN, Title, Author must have a value!");
                }
                if(book_->available_copies_ <= 0){
                    throw std::invalid_argument("avaliable copies must be positive!");
                }

                return std::move(book_);
            }
        private:
            std::unique_ptr<Book> book_;
    };

    static BookBuilder create(){
        return {};
    }

    static std::unique_ptr<Book> findById(int book_id);
    static std::unique_ptr<Book> findByIsbn(const std::string &isbn);
    static std::vector<std::unique_ptr<Book>> findAll(int page, int pagesize);
    static int count();
    static std::vector<std::unique_ptr<Book>> search(const std::string& keyword);

    bool save();
    bool update();
    bool remove();

    bool borrow();
    bool return_book();

    [[nodiscard]] int getId() const { return id_; }
    [[nodiscard]] const std::string& getIsbn() const { return isbn_; }
    [[nodiscard]] const std::string& getTitle() const { return title_; }
    [[nodiscard]] const std::string& getAuthor() const { return author_; }
    [[nodiscard]] const std::string& getPublisher() const { return publisher_; }
    [[nodiscard]] const std::string& getPublishDate() const { return publish_date_; }
    [[nodiscard]] const std::string& getCategory() const { return category_; }
    [[nodiscard]] int getTotalCopies() const { return total_copies_; }
    [[nodiscard]] int getAvailableCopies() const { return available_copies_; }
private:
    int id_{0};
    std::string isbn_;
    std::string title_;
    std::string author_;
    std::string publisher_;
    std::string publish_date_;
    std::string category_;
    int total_copies_{0};
    int available_copies_{0};

    Book() =default;
    friend class BookBuilder;
};