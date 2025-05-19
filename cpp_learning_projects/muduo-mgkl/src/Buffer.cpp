#include "Buffer.hpp"

#include <error.h>
#include <sys/uio.h>
#include <unistd.h>

ssize_t Buffer::readFd(int fd, int * saveErrno) {
    // 64 kB
    char extrabuf[65536] = { 0 };

    /**
     * struct iovec {
     *      void * iov_base;
     *      size_t iov_len;
     * };
     */
    struct iovec vec[2];
    const size_t writable = writableBytes();

    // 可写空间
    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writable;

    // 栈空间
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    const int iovcnt = (writable < sizeof(extrabuf) ? 2 : 1);
    const ssize_t n = ::readv(fd, vec, iovcnt);

    if (n < 0) {
        *saveErrno = errno;
    } else if (n <= writable) {
        writerIndex_ += n;
    } else {
        // extrabuf 中也写入了数据
        // 写入数据字节数为 n - writable 
        writerIndex_ = buffer_.size();
        append(extrabuf, n - writable);
    }

    return n;
}

ssize_t Buffer::writeFd(int fd, int * saveErrno) {
    ssize_t n = ::write(fd, peek(), readableBytes());

    if (n < 0) {
        *saveErrno = errno;
    }

    return n;
}