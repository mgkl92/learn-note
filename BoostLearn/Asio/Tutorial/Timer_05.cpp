//
// timer.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <functional>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>

// Timer 5: Synchroning completion handlers in multithreaded programs
// Link: https://www.boost.org/doc/libs/latest/doc/html/boost_asio/tutorial/tuttimer4/src.html
// Description: How to use the strand class template to synchronise completion handlers in a multithreaded program.

class printer {
public:
    printer(boost::asio::io_context &io_context) :
        strand_(boost::asio::make_strand(io_context)),
        timer1_(io_context, boost::asio::chrono::seconds(1)),
        timer2_(io_context, boost::asio::chrono::seconds(1)),
        count_(0) {
        // When initiating the asynchronous operations,
        // each completion handler is "bound" to an boost::asio::strand<boost::asio::io_context::executor_type> object.
        timer1_.async_wait(boost::asio::bind_executor(strand_, std::bind(&printer::print1, this)));
        timer2_.async_wait(boost::asio::bind_executor(strand_, std::bind(&printer::print2, this)));
    }

    ~printer() {
        std::cout << "Final count is " << count_ << std::endl;
    }

    void print1() {
        if (count_ < 10) {
            std::cout << "[Timer 1] count = " << count_ << std::endl;
            ++count_;

            timer1_.expires_at(timer1_.expiry() + boost::asio::chrono::seconds(1));
            timer1_.async_wait(boost::asio::bind_executor(strand_, std::bind(&printer::print1, this)));
        }
    }

    void print2() {
        if (count_ < 10) {
            std::cout << "[Timer 2] count = " << count_ << std::endl;
            ++count_;

            timer2_.expires_at(timer2_.expiry() + boost::asio::chrono::seconds(1));
            timer2_.async_wait(boost::asio::bind_executor(strand_, std::bind(&printer::print2, this)));
        }
    }

private:
    // The strand class template is an executor adapter that guarantees that,
    // for those handlers that are dispatched through it,
    // an executing handler will be allowed to complete before the next one is started.
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    boost::asio::steady_timer timer1_;
    boost::asio::steady_timer timer2_;
    int count_;
};

int main(int argc, char const *argv[]) {
    boost::asio::io_context io_context;
    printer p(io_context);

    // The main function now causes boost::asio::io_context::run()
    // to be called from two threads: the main thread and one additional thread.
    std::thread t([&]() { io_context.run(); });
    io_context.run();

    t.join();

    return 0;
}
