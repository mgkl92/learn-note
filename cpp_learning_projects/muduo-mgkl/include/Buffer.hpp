#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <stddef.h>

using std::string;
using std::vector;

class Buffer {
public:
    static const size_t kCheapPrepend = 8;

    // 缓冲区初始大小
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize):
        buffer_(kCheapPrepend + initialSize),
        readerIndex_(kCheapPrepend),
        writerIndex_(kCheapPrepend) {
    }

    // 缓冲区中可读数据字节数目
    size_t readableBytes() const {
        return writerIndex_ - readerIndex_;
    }

    // 缓冲区中可写数据字节数目
    size_t writableBytes() const {
        return buffer_.size() - writerIndex_;
    }

    // ？
    size_t prependableBytes() const {
        return readerIndex_;
    }

    // 缓冲区中可读数据的起始地址
    const char * peek() const {
        return begin() + readerIndex_;
    }

    // 变更读指针
    void retrieve(size_t len) {
        if (len < readableBytes()) {
            readerIndex_ += len;
        } else {
            retrieveAll();
        }
    }

    void retrieveAll() {
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;
    }

    // 从缓冲区中读取数据
    string retrieveAsString(size_t len) {
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    string retrieveAllAsString() {
        return retrieveAsString(readableBytes());
    }

    // 缓冲区扩容
    void ensureWritableBytes(size_t len) {
        if (writableBytes() < len) {
            makeSpace(len);
        }
    }

    // 向缓冲区中追加数据
    void append(const char * data, size_t len) {
        ensureWritableBytes(len);
        std::copy(data, data + len, beginWrite());
        writerIndex_ += len;
    }

    char * beginWrite() {
        return begin() + writerIndex_;
    }

    const char * beginWrite() const {
        return begin() + writerIndex_;
    }

    ssize_t readFd(int fd, int * saveErrno);

    ssize_t writeFd(int fd, int * saveErrno);

private:
    char * begin() {
        return &(*buffer_.begin());
    }

    const char * begin() const {
        return &(*buffer_.begin());
    }

    void makeSpace(size_t len) {
        // 判断缓冲区中是否存在足够的剩余空间
        if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
            buffer_.resize(writerIndex_ + len);
        } else {
            size_t readable = readableBytes();

            // 对缓冲区中旧数据进行腾挪
            std::copy(begin() + readerIndex_, begin() + writerIndex_, begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
        }
    }

    vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
};