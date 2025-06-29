//
// timer.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Timer 4: Using a member function as a completion handler
// Link: https://www.boost.org/doc/libs/latest/doc/html/boost_asio/tutorial/tuttimer4/src.html
// Description: How to use a class member function as a completion handler.

#include <functional>
#include <iostream>
#include <boost/asio.hpp>
// #include <boost/bind.hpp>

class printer {
public:
    printer(boost::asio::io_context &io_context) :
        timer_(io_context, boost::asio::chrono::seconds(1)), count_(0) {
        timer_.async_wait(std::bind(&printer::print, this));
    }

    ~printer() {
        std::cout << "Final count is " << count_ << std::endl;
    }

    void print() {
        if (count_ < 5) {
            std::cout << "count = " << count_ << std::endl;
            ++count_;

            timer_.expires_at(timer_.expiry() + boost::asio::chrono::seconds(1));
            timer_.async_wait(std::bind(&printer::print, this));
        }
    }

private:
    boost::asio::steady_timer timer_;
    int count_;
};

int main(int argc, char const *argv[]) {
    boost::asio::io_context io_context;

    printer p(io_context);

    io_context.run();
    return 0;
}
