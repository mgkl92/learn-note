#include "ConnectionPool.h"

#include "json.hpp"

#include <fstream>
#include <unistd.h>

ConnectionPool& ConnectionPool::GetConnectionPool() {
    static ConnectionPool pool;
    return pool;
}

std::shared_ptr<Connection> ConnectionPool::GetConnection() {
    std::unique_lock<std::mutex> lock(mtx_);
    bool ready = cv_.wait_for(lock,
        std::chrono::microseconds(connectionTimeout_),
        [this]() {
            return !connectionQueue_.empty();
        });

    if (!ready) {
        // timeout
        return nullptr;
    }

    std::shared_ptr<Connection> res(connectionQueue_.front(),
        [this](Connection* conn) {
            std::unique_lock<std::mutex> lock(mtx_);
            if (!exit_) {
                conn->RefreshAliveTime();
                connectionQueue_.push(conn);
            }
            else {
                delete conn;
            }
        });

    connectionQueue_.pop();
    cv_.notify_all();

    return res;
}

ConnectionPool::~ConnectionPool() {
    if (!exit_) {
        close();
    }
}

void ConnectionPool::start()
{
    if (!LoadConfigFile()) {
        LOG_ERROR("JSON Config Error.");
        return;
    }

    for (int i = 0; i < minSize_; ++i) {
        AddConnection();
    }

    std::thread producer(std::bind(&ConnectionPool::ProduceConnectionTask, this));
    producer.detach();

    std::thread scanner(std::bind(&ConnectionPool::ScannerConnectionTask, this));
    scanner.detach();

    exit_ = false;
    cv_.notify_all();
}

void ConnectionPool::close() {
    exit_ = true;

    std::unique_lock<std::mutex> lock(mtx_);
    while (!connectionQueue_.empty())
    {
        Connection* conn = connectionQueue_.front();
        connectionQueue_.pop();

        delete conn;
    }

    cv_.notify_all();
}

bool ConnectionPool::LoadConfigFile() {
    std::ifstream ifs("../config/dbconfig.json");
    nlohmann::json js;

    ifs >> js;
    if (!js.is_object()) {
        LOG_ERROR("JSON is Not Object.");
        return false;
    }
    if (!(js["ip"].is_string() &&
        js["port"].is_number() &&
        js["user"].is_string() &&
        js["pwd"].is_string() &&
        js["db"].is_string() &&
        js["minSize"].is_number() &&
        js["maxSize"].is_number() &&
        js["maxIdleTime"].is_number() &&
        js["timeout"].is_number())) {
        LOG_ERROR("JSON The data type does not match.");
        return false;
    }

    ip_ = js["ip"].get<std::string>();
    port_ = js["port"].get<uint16_t>();
    user_ = js["user"].get<std::string>();
    password_ = js["pwd"].get<std::string>();
    dbname_ = js["db"].get<std::string>();
    minSize_ = js["minSize"].get<size_t>();
    maxSize_ = js["maxSize"].get<size_t>();
    maxIdleTime_ = js["maxIdleTime"].get<size_t>();
    connectionTimeout_ = js["timeout"].get<size_t>();

    return true;
}

void ConnectionPool::ProduceConnectionTask() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this]() {
            return exit_ || connectionQueue_.empty();
            });

        if (exit_) {
            break;
        }

        if (connectionCount_ < maxSize_) {
            AddConnection();
        }

        cv_.notify_all();
    }
}

void ConnectionPool::ScannerConnectionTask() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx_);
        std::chrono::time_point<std::chrono::steady_clock> ddl = std::chrono::steady_clock::now() + std::chrono::seconds(maxIdleTime_);
        bool ready = cv_.wait_for(lock, std::chrono::seconds(maxIdleTime_), [this, &ddl]()
            {
                std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
                return exit_ || (now > ddl);
            });

        if (!ready) {
            // timeout
            return;
        }
        if (exit_) {
            return;
        }

        while (connectionCount_ > minSize_) {
            Connection* conn = connectionQueue_.front();
            if (conn->GetAliveTime() >= maxIdleTime_ * 1000) {
                connectionQueue_.pop();
                --connectionCount_;
                delete conn;
            }
            else {
                break;
            }
        }
    }
}

void ConnectionPool::AddConnection() {
    Connection* conn = new Connection();
    conn->Connect(ip_, port_, user_, password_, dbname_);
    conn->RefreshAliveTime();
    connectionQueue_.push(conn);
    ++connectionCount_;
}