//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Daytime 2: A synchronous TCP daytime server
// Link: https://www.boost.org/doc/libs/latest/doc/html/boost_asio/tutorial/tutdaytime2.html
// Description: How to use asio to implement a synchronous server application with TCP.

#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

std::string make_daytiem_string() {
    using namespace std;

    time_t now = time(0);
    return ctime(&now);
}

int main(int argc, char const *argv[]) {
    try {
        boost::asio::io_context io_context;

        // A ``ip::tcp::acceptor`` object needs to be created to listen for new connections.
        // It is initialised to listen on TCP port 13, for IP version 4.
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 13));

        for (;;) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            std::string message = make_daytiem_string();

            boost::system::error_code ignored_error;
            boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
