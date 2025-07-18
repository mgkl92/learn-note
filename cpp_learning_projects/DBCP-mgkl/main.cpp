#include <iostream>
#include <memory>
#include <unistd.h>

#include "Connection.h"
#include "ConnectionPool.h"

using namespace std;
// 1. 单线程: 使用/不使用连接池
// 2. 多线程: 使用/不使用连接池

static const std::string host = "127.0.0.1";
static const uint16_t port = 3306;
static const std::string user = "root";
static const std::string password = "123456";
static const std::string dbname = "testdb";

void op1(int begin, int end) {
    for (int i = begin; i < end; ++i) {
        Connection conn;
        conn.Connect(host, port, user, password, dbname);
        char sql[1024] = { 0 };
        sprintf(sql, "insert into person values(%d, 25, 'man', 'tom')", i);
        conn.Update(sql);
    }
}

void op2(ConnectionPool* pool, int begin, int end) {
    for (int i = begin; i < end; ++i) {
        shared_ptr<Connection> conn = pool->GetConnection();
        char sql[1024] = { 0 };
        sprintf(sql, "insert into person values(%d, 25, 'man', 'tom')", i);
        conn->Update(sql);
    }
}

void test1(int op) {
    switch (op) {
    case 1: {
        // 非连接池, 单线程, 用时: 21037278300 纳秒, 21037 毫秒
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        op1(0, 5000);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        auto length = end - begin;
        cout << "非连接池, 单线程, 用时: " << length.count() << " 纳秒, " << length.count() / 1000000 << " 毫秒" << endl;
    }
          break;
    case 2: {
        // 连接池, 单线程, 用时: 8838406500 纳秒, 8838 毫秒
        ConnectionPool& pool = ConnectionPool::GetConnectionPool();
        pool.start();

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        op2(&pool, 0, 5000);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        auto length = end - begin;
        cout << "连接池, 单线程, 用时: " << length.count() << " 纳秒, " << length.count() / 1000000 << " 毫秒" << endl;

        pool.close();
    }
          break;
    }
}

void test2(int op)
{
    switch (op) {
    case 3: {
        // 非连接池, 多线程, 用时: 75244742100 纳秒, 75244 毫秒
        Connection conn;
        conn.Connect(host, port, user, password, dbname);
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        thread t1(op1, 0, 1000);
        thread t2(op1, 1000, 2000);
        thread t3(op1, 2000, 3000);
        thread t4(op1, 3000, 4000);
        thread t5(op1, 4000, 5000);
        t1.join();
        t2.join();
        t3.join();
        t4.join();
        t5.join();
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        auto length = end - begin;
        cout << "非连接池, 多线程, 用时: " << length.count() << " 纳秒, " << length.count() / 1000000 << " 毫秒" << endl;
    }
          break;
    case 4: {
        // 连接池, 多线程, 用时: 11489727300 纳秒, 11489 毫秒
        ConnectionPool& pool = ConnectionPool::GetConnectionPool();
        pool.start();

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        thread t1(op2, &pool, 0, 1000);
        thread t2(op2, &pool, 1000, 2000);
        thread t3(op2, &pool, 2000, 3000);
        thread t4(op2, &pool, 3000, 4000);
        thread t5(op2, &pool, 4000, 5000);
        t1.join();
        t2.join();
        t3.join();
        t4.join();
        t5.join();
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        auto length = end - begin;
        cout << "连接池, 多线程, 用时: " << length.count() << " 纳秒, " << length.count() / 1000000 << " 毫秒" << endl;

        pool.close();
    }
          break;
    }
}

int query() {
    Connection conn;
    bool tt = conn.Connect("127.0.0.1", 3306, "root", "123456", "testdb");
    cout << "tt:  " << tt << endl;
    string sql = "insert into person values(7, 25, 'man', 'tom')";
    bool flag = conn.Update(sql);
    cout << "flag value:  " << flag << endl;
    return 0;
}

int main() {
    while (true) {
        std::cout << "---------------------------" << std::endl;
        std::cout << "1) 非连接池，单线程\n2) 连接池，单线程\n3) 非连接池，多线程\n4) 连接池，多线程\n";
        std::cout << "请输入数字 1 ~ 4:";
        int op;
        std::cin >> op;
        std::cout << "测试进行中..." << std::endl;
        switch (op) {
        case 1:
        case 2: {
            test1(op);
        }
              break;
        case 3:
        case 4: {
            test2(op);
        }
              break;
        }

        if (op < 1 || op > 4) {
            std::cout << "测试结束！" << std::endl;
            break;
        }
    }
    return 0;
}
// 连接池, 多单线程, 用时: 11489727300 纳秒, 11489 毫秒
// 非连接池, 多单线程, 用时: 75244742100 纳秒, 75244 毫秒
// 连接池, 单线程, 用时: 37506828200 纳秒, 37506 毫秒
// 非连接池, 单线程, 用时: 58250833100 纳秒, 58250 毫秒