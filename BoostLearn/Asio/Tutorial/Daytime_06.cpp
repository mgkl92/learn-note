//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Daytime 6: An asynchronous UDP daytime server
// Link: https://www.boost.org/doc/libs/latest/doc/html/boost_asio/tutorial/tutdaytime7.html
// Description: How to use asio to implement an asynchronous server application with UDP.

#include <array>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

std::string make_daytime_string() {
    using namespace std;

    time_t now = time(0);
    return ctime(&now);
}

class udp_server {
public:
    udp_server(boost::asio::io_context &io_context) :
        socket_(io_context, udp::endpoint(udp::v4(), 13)) {
        start_receive();
    }

private:
    // The function ``ip::udp::socket::async_receive_from()`` will cause the application to listen in the background for a new request.
    void start_receive() {
        socket_.async_receive_from(boost::asio::buffer(recv_buffer_),
                                   remote_endpoint_,
                                   std::bind(&udp_server::handle_receive,
                                             this,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
    }

    // The function ``ip::udp::socket::async_receive_from()`` will cause the application to listen in the background for a new request.
    void handle_receive(const boost::system::error_code &error,
                        std::size_t /*bytes_transferred*/) {
        if (!error) {
            std::shared_ptr<std::string> message = std::make_shared<std::string>(make_daytime_string());
            socket_.async_send_to(boost::asio::buffer(*message),
                                  remote_endpoint_,
                                  std::bind(&udp_server::handle_send,
                                            this,
                                            message,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));

            start_receive();
        }
    }

    void handle_send(std::shared_ptr<std::string> /*message*/,
                     const boost::system::error_code & /*error*/,
                     std::size_t /*bytes_transferred*/) {
    }

    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    std::array<char, 1> recv_buffer_;
};

int main(int argc, char const *argv[]) {
    try {
        boost::asio::io_context io_context;

        udp_server server(io_context);
        io_context.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
