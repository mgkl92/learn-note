#pragma once

#include <iostream>
#include <string>

using std::string;

class Timestamp {
public:
    Timestamp();
    explicit Timestamp(int64_t microSecondsSinceEpoch);
    static Timestamp now();
    string toString() const;

private:
    int64_t microSecondsSinceEpoch_;
};