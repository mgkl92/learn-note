//
// timer.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Timer 1: Using a timer synchronously
// Link: https://www.boost.org/doc/libs/latest/doc/html/boost_asio/tutorial/tuttimer1.html
// Description: How to perform a blocking wait on a timer. 

#include <iostream>
#include <boost/asio.hpp>

int main(int argc, char const *argv[]) {
    // An I/O execution context provides access to I/O functionality.
    boost::asio::io_context io;

    // A timer is always in one of two states: "expired" or "not expired".
    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));

    // If the steady_timer::wait() function is called on an expired timer, it will return immediately.
    t.wait();

    std::cout << "Hello, world!" << std::endl;

    return 0;
}
