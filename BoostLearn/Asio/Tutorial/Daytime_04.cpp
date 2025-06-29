//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Daytime 4: A synchronous UDP daytime client
// Link: https://www.boost.org/doc/libs/latest/doc/html/boost_asio/tutorial/tutdaytime4.html
// Description: How to use asio to implement a client application with UDP.

#include <array>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

int main(int argc, char const *argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }

        boost::asio::io_context io_context;
        udp::resolver resolver(io_context);

        // Use an ip::udp::resolver object to find the correct remote endpoint to use based on the host and service names.
        // ``service``: A string identifying the requested service(This may be a descriptive name or a numeric string corresponding to a port number).
        // The ``ip::udp::resolver::resolve()`` function is guaranteed to return at least one endpoint in the list if it does not fail.
        udp::endpoint receiver_endpoint = *(resolver.resolve(udp::v4(), argv[1], "daytime").begin());

        udp::socket socket(io_context);
        socket.open(udp::v4());

        std::array<char, 1> send_buf = {0};
        socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

        // The endpoint on our side that receives the server's response will be initialised by ``ip::udp::socket::receive_from()``.
        std::array<char, 128> recv_buf;
        udp::endpoint sender_endpoint;
        size_t len = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);

        std::cout.write(recv_buf.data(), len);
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
