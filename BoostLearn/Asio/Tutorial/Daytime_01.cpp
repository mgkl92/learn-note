//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Daytime 1: A synchronous TCP daytime client
// Link: https://www.boost.org/doc/libs/latest/doc/html/boost_asio/tutorial/tutdaytime1.html
// Description: How to use asio to implement a client application with TCP.

#include <array>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(int argc, char const *argv[]) {
    try {
        // The user needs to specify the server.
        if (argc != 2) {
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }

        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);

        // A resolver takes a host name and service name and turns them into a list of endpoints.
        // The list of endpoints is returned using an object of type ip::tcp::resolver::results_type
        // This object is a range, with ``begin()`` and ``end()`` member functions that may be used for iterating over the results.
        tcp::resolver::results_type endpoints = resolver.resolve(argv[1], "daytime");

        tcp::socket socket(io_context);
        // The list of endpoints obtained above may contain both IPv4 and IPv6 endpoints,
        // so we need to try each of them until we find one that works.
        // This keeps the client program independent of a specific IP version.
        // The ``boost::asio::connect()`` function does this for us automatically.
        boost::asio::connect(socket, endpoints);

        for (;;) {
            std::array<char, 128> buf;
            boost::system::error_code error;

            size_t len = socket.read_some(boost::asio::buffer(buf), error);
            // When the server closes the connection,
            // the ``ip::tcp::socket::read_some()`` function will exit with the ``boost::asio::error::eof`` error
            if (error == boost::asio::error::eof) {
                break;
            } else if (error) {
                throw boost::system::system_error(error);
            } else {
                std::cout.write(buf.data(), len);
            }
        }
    } catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << '\n';
    }

    return 0;
}
