#pragma once

#include "NonCopyable.hpp"

#include <string>

// 定义日志输出宏
// do {} while(0) 可避免在使用宏函数时发生错误
// do while 循环能保证循环体至少执行一次
// do {} while(0) 保证循环体只执行一次
#define LOG_INFO(logmsgFormat, ...)                       \
    do {                                                  \
        Logger & logger = Logger::instance();             \
        logger.setLogLevel(INFO);                         \
        char buf[1024] = { 0 };                           \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf);                                  \
    } while (0)

#define LOG_ERROR(logmsgFormat, ...)                      \
    do {                                                  \
        Logger & logger = Logger::instance();             \
        logger.setLogLevel(ERROR);                        \
        char buf[1024] = { 0 };                           \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf);                                  \
    } while (0)

#define LOG_FATAL(logmsgFormat, ...)                      \
    do {                                                  \
        Logger & logger = Logger::instance();             \
        logger.setLogLevel(FATAL);                        \
        char buf[1024] = { 0 };                           \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf);                                  \
    } while (0)

// 控制 DEBUG 日志信息的输出
#ifdef MUDEBUG
#define LOG_DEBUG(logmsgFormat, ...)                      \
    do {                                                  \
        Logger & logger = Logger::instance();             \
        logger.setLogLevel(DEBUG);                        \
        char buf[1024] = { 0 };                           \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf);                                  \
    } while (0)
#else
#define LOG_DEBUG(logmsgFormat, ...)
#endif

// 定义日志级别
enum LogLevel {
    INFO,
    ERROR,
    FATAL,
    DEBUG
};

class Logger: NonCopyable {
public:
    static Logger & instance();

    void setLogLevel(int level);

    void log(std::string mgs);

private:
    int logLevel_;
};