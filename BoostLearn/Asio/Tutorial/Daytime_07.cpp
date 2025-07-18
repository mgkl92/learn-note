//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Daytime 7: An asynchronous UDP daytime server
// Link: https://www.boost.org/doc/libs/latest/doc/html/boost_asio/tutorial/tutdaytime7.html

#include <array>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

std::string make_daytime_string() {
    using namespace std;

    time_t now = time(0);
    return ctime(&now);
}

class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
public:
    typedef std::shared_ptr<tcp_connection> tcp_connection_ptr;

    static tcp_connection_ptr create(boost::asio::io_context &io_context) {
        return tcp_connection_ptr(new tcp_connection(io_context));
    }

    tcp::socket &socket() {
        return socket_;
    }

    void start() {
        message_ = make_daytime_string();

        boost::asio::async_write(socket_,
                                 boost::asio::buffer(message_),
                                 std::bind(&tcp_connection::handle_write,
                                             shared_from_this()));
    }

private:
    tcp_connection(boost::asio::io_context &io_context) :
        socket_(io_context) {
    }

    void handle_write() {
    }

    tcp::socket socket_;
    std::string message_;
};

class tcp_server {
public:
    tcp_server(boost::asio::io_context &io_context) :
        io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), 13)) {
        start_accept();
    }

private:
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

        start_accept();
    }

    boost::asio::io_context &io_context_;
    tcp::acceptor acceptor_;
};

class udp_server {
public:
    udp_server(boost::asio::io_context &io_conteext) :
        socket_(io_conteext, udp::endpoint(udp::v4(), 13)) {
        start_receive();
    }

private:
    void start_receive() {
        socket_.async_receive_from(boost::asio::buffer(recv_buffer_),
                                   remote_endpoint_,
                                   std::bind(&udp_server::handle_receive,
                                               this,
                                               boost::asio::placeholders::error));
    }

    void handle_receive(const boost::system::error_code &error) {
        if (!error) {
            auto message = std::make_shared<std::string>(make_daytime_string());

            socket_.async_send_to(boost::asio::buffer(*message),
                                  remote_endpoint_,
                                  std::bind(&udp_server::handle_send,
                                              this, message));

            start_receive();
        }
    }

    void handle_send(std::shared_ptr<std::string> /*message*/) {
    }

    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    std::array<char, 1> recv_buffer_;
};

int main(int argc, char const *argv[]) {
    try {
        boost::asio::io_context io_context;

        tcp_server server1(io_context);
        udp_server server2(io_context);

        io_context.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
