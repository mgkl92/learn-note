//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Daytime 3: An asynchronous TCP daytime server
// Link: https://www.boost.org/doc/libs/latest/doc/html/boost_asio/tutorial/tutdaytime3.html
// Description: How to use asio to implement asynchronous server application with TCP.

#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

std::string make_daytime_string() {
    using namespace std;

    time_t now = time(0);
    return ctime(&now);
}

// We will use ``std::shared_ptr`` and ``std::enable_shared_from_this`` 
// because we want to keep the tcp_connection object alive as long as there is an operation that refers to it.
class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
public:
    typedef std::shared_ptr<tcp_connection> tcp_connection_ptr;

    static tcp_connection_ptr create(boost::asio::io_context &io_context) {
        return std::shared_ptr<tcp_connection>(new tcp_connection(io_context));
    }

    tcp::socket &socket() {
        return socket_;
    }

    void start() {
        message_ = make_daytime_string();

        boost::asio::async_write(socket_,
                                 boost::asio::buffer(message_),
                                 std::bind(&tcp_connection::handle_write,
                                             shared_from_this(),
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
    }

private:
    tcp_connection(boost::asio::io_context &io_context) :
        socket_(io_context) {
    }

    void handle_write(const boost::system::error_code /*error*/, size_t /*bytes_transferred*/) {
    }

    tcp::socket socket_;
    std::string message_;
};

class tcp_server {
public:
    tcp_server(boost::asio::io_context &io_context) :
        io_context_(io_context),
        acceptor_(io_context, tcp::endpoint(tcp::v4(), 13)) {
        start_accept();
    }

private:
    // The function ``start_accept()`` creates a socket
    // and initiates an asynchronous accept operation to wait for a new connection.
    void start_accept() {
        tcp_connection::tcp_connection_ptr new_connection = tcp_connection::create(io_context_);

        acceptor_.async_accept(new_connection->socket(),
                               std::bind(&tcp_server::handle_accept,
                                           this,
                                           new_connection,
                                           boost::asio::placeholders::error));
    }

    void handle_accept(tcp_connection::tcp_connection_ptr new_connection,
                       const boost::system::error_code &error) {
        if (!error) {
            new_connection->start();
        }

        // Call ``start_accept()`` to initiate the next accept operation.
        start_accept();
    }

    boost::asio::io_context &io_context_;
    tcp::acceptor acceptor_;
};

int main(int argc, char const *argv[]) {
    try {
        boost::asio::io_context io_context;
        tcp_server server(io_context);

        io_context.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
