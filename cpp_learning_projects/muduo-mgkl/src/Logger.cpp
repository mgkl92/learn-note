#include "Logger.hpp"

#include "Timestamp.hpp"

#include <iostream>

// 单例模式
// C++ 11 后保证静态成员变量的定义是线程安全的
Logger & Logger::instance() {
    static Logger logger;
    return logger;
}

// 设置日志级别
void Logger::setLogLevel(int level) {
    logLevel_ = level;
}

// 打印日志信息
void Logger::log(std::string msg) {
    std::string prefix = "";

    switch (logLevel_) {
    case INFO:
        prefix = "[INFO]";
        break;
    case ERROR:
        prefix = "[ERROR]";
        break;
    case FATAL:
        prefix = "[FATAL]";
        break;
    case DEBUG:
        prefix = "[DEBUG]";
        break;
    }

    std::cout << prefix + Timestamp::now().toString() << " : " << msg << std::endl;
}