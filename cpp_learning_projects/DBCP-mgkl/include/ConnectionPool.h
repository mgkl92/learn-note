#pragma once

#include "Connection.h"
#include "noncopyable.hpp"

#include <memory>
#include <queue>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>

class ConnectionPool : private noncopyable {
public:
    static ConnectionPool& GetConnectionPool();

    std::shared_ptr<Connection> GetConnection();

    ~ConnectionPool();

    void start();

    void close();

private:
    ConnectionPool() = default;

    bool LoadConfigFile();

    void ProduceConnectionTask();

    void ScannerConnectionTask();

    void AddConnection();

private:
    std::string ip_;

    uint16_t port_;

    std::string user_;

    std::string password_;

    std::string dbname_;

    size_t minSize_;

    size_t maxSize_;

    size_t maxIdleTime_;

    size_t connectionTimeout_;

    std::queue<Connection*> connectionQueue_;

    std::mutex mtx_;

    std::atomic_int connectionCount_;

    std::condition_variable cv_;

    std::atomic_bool exit_;
};