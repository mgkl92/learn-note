#include "Buffer.h"
#include <string.h>
#include <iostream>

Buffer::Buffer() {
}

Buffer::~Buffer() {
}

void Buffer::append(const char *str_, int size_) {
    for (int i = 0; i < size_; ++i) {
        if (str_[i] == '\0') break;
        buf.push_back(str_[i]);
    }
}

ssize_t Buffer::size() {
    return buf.size();
}

const char * Buffer::c_str() {
    return buf.c_str();
}

void Buffer::clear() {
    buf.clear();
}

void Buffer::getline() {
    buf.clear();
    std::getline(std::cin, buf);
}