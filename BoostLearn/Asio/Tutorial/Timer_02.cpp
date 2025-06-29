//
// timer.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Timer 2: Using a timer asynchronously
// Link: https://www.boost.org/doc/libs/latest/doc/html/boost_asio/tutorial/tuttimer2.html
// Description: How to use asio's asynchronous to perform an asynchronous wait on the timer. 

#include <iostream>
#include <boost/asio.hpp>

// A completion token, which determines how the result will be delivered to a completion handler when an asynchronous operation completes.
void print(const boost::system::error_code &) {
    std::cout << "Hello, world!" << std::endl;
}

int main(int argc, char const *argv[]) {
    boost::asio::io_context io;

    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));

    // Call the steady_timer::async_wait() function to perform an asynchronous wait.
    t.async_wait(&print);

    // The boost::asio::io_context::run() function will also continue to run while there is still "work" to do.
    // Otherwise, the `main` will return directly.
    io.run();

    return 0;
}
