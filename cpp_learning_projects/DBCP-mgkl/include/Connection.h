#pragma once

#include "Log.hpp"

#include <chrono>
#include <string>

#include <mysql/mysql.h>

class Connection {
public:
    Connection();

    ~Connection();

    bool Connect(const std::string& ip,
        const uint16_t port,
        const std::string& user,
        const std::string& password,
        const std::string& dbname);

    bool Update(const std::string& sql);

    MYSQL_RES* Query(const std::string& sql);

    void RefreshAliveTime();

    long long GetAliveTime();

private:
    MYSQL* conn_;

    std::chrono::time_point<std::chrono::steady_clock> aliveTime_;
};
