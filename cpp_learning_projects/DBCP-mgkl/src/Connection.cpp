#include "Connection.h"

Connection::Connection() {
    conn_ = mysql_init(nullptr);
}

Connection::~Connection() {
    if (conn_) {
        mysql_close(conn_);
    }
}
bool Connection::Connect(const std::string& ip,
    const uint16_t port,
    const std::string& user,
    const std::string& password,
    const std::string& dbname) {
    if (conn_ = mysql_real_connect(conn_,
        ip.c_str(),
        user.c_str(),
        password.c_str(),
        dbname.c_str(),
        port, nullptr, 0);
        !conn_) {
        LOG_ERROR("MySQL Connection Error");
        return false;
    }

    return true;
}

bool Connection::Update(const std::string& sql) {
    if (mysql_query(conn_, sql.c_str())) {
        LOG_INFO("MySQL Update: \" %s \", Error: %d.", sql.c_str(), mysql_errno(conn_));
        return false;
    }

    return true;
}

MYSQL_RES* Connection::Query(const std::string& sql) {
    if (mysql_query(conn_, sql.c_str())) {
        LOG_INFO("MySQL Update: \" %s \", Error: %d.", sql.c_str(), mysql_errno(conn_));
        return nullptr;
    }

    return mysql_use_result(conn_);
}

void Connection::RefreshAliveTime() {
    aliveTime_ = std::chrono::steady_clock::now();
}

long long Connection::GetAliveTime() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - aliveTime_).count();
}