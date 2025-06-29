//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Daytime 5: A synchronous UDP daytime server
// Link: https://www.boost.org/doc/libs/latest/doc/html/boost_asio/tutorial/tutdaytime5.html
// Description: How to use asio to implement a synchronous server application with UDP.

#include <array>
#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

std::string make_daytime_string() {
    using namespace std;

    time_t now = time(0);
    return ctime(&now);
}

int main(int argc, char const *argv[]) {
    try {
        // Create an ``ip::udp::socket`` object to receive requests on UDP port 13.
        boost::asio::io_context io_context;
        udp::socket socket(io_context, udp::endpoint(udp::v4(), 13));

        for (;;) {
            // Wait for a client to initiate contact with us. 
            // The remote_endpoint object will be populated by ``ip::udp::socket::receive_from()``.
            std::array<char, 1> recv_buf;
            udp::endpoint remote_endpoint;
            socket.receive_from(boost::asio::buffer(recv_buf), remote_endpoint);

            std::string message = make_daytime_string();

            boost::system::error_code ignored_error;
            socket.send_to(boost::asio::buffer(message), remote_endpoint, 0, ignored_error);
        }
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
