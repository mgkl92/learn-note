//
// timer.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Timer 3: Using a timer asynchronously
// Link: https://www.boost.org/doc/libs/latest/doc/html/boost_asio/tutorial/tuttimer2.html
// Description: How to pass additional parameters to your handler function.

#include <iostream>
#include <functional>
#include <boost/asio.hpp>

void print(const boost::system::error_code & /*e*/,
           boost::asio::steady_timer *t, int *count) {
    // By not starting a new asynchronous wait on the timer when count reaches 5,
    // the io_context will run out of work and stop running.
    if (*count < 5) {
        std::cout << "count = " << *count << std::endl;
        ++(*count);

        // Move the expiry time for the timer along by one second from the previous expiry time.
        t->expires_at(t->expiry() + boost::asio::chrono::seconds(1));
        t->async_wait(std::bind(print, boost::asio::placeholders::error, t, count));
    }
}

int main() {
    boost::asio::io_context io;
    int count = 0;

    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(1));
    t.async_wait(std::bind(print, boost::asio::placeholders::error, &t, &count));

    io.run();
    std::cout << "Final count is " << count << std::endl;

    return 0;
}