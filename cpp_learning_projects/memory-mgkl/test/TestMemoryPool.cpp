#include "../include/MemoryPool.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <stdexcept>

using namespace mgkl;

class P1 {
    int id_;
};

class P2 {
    int id_[5];
};

class P3 {
    int id_[10];
};

class P4 {
    int id_[20];
};

void BenchmarkMemoryPool(size_t ntimes, size_t nworks, size_t nrounds) {
    std::vector<std::thread> vthread(nworks);
    size_t cost = 0;

    auto work = [&]() {
        size_t start = clock();

        for (size_t i = 0; i < ntimes; ++i) {
            P1 * p1 = newElement<P1>();
            deleteElement<P1>(p1);

            P2 * p2 = newElement<P2>();
            deleteElement<P2>(p2);

            P3 * p3 = newElement<P3>();
            deleteElement<P3>(p3);

            P4 * p4 = newElement<P4>();
            deleteElement<P4>(p4);
        }

        cost += clock() - start;
    };

    for (size_t k = 0; k < nworks; ++k) {
        vthread[k] = std::thread(work);
        vthread[k].join();
    }

    printf("%lu个线程并发执行%lu轮次，每轮次newElement&deleteElement %lu次，总计花费：%lu ms\n", nworks, nrounds, ntimes, cost);
}

void BenchmarkNew(size_t ntimes, size_t nworks, size_t nrounds) {
    std::vector<std::thread> vthread(nworks);
    size_t cost = 0;

    auto work = [&]() {
        for (size_t j = 0; j < nrounds; ++j) {
            size_t start = 0;
            for (size_t i = 0; i < ntimes; ++i) {
                P1 * p1 = new P1;
                delete p1;

                P2 * p2 = new P2;
                delete p2;

                P3 * p3 = new P3;
                delete p3;

                P4 * p4 = new P4;
                delete p4;
            }

            cost += clock() - start;
        }
    };

    for (size_t k = 0; k < nworks; ++k) {
        vthread[k] = std::thread(work);
        vthread[k].join();
    }

    printf("%lu个线程并发执行%lu轮次，每轮次malloc&free %lu次，总计花费：%lu ms\n", nworks, nrounds, ntimes, cost);
}

int main(int argc, const char * argv[]) {
    if (argc < 4) {
        throw std::invalid_argument("No sufficient arguments.");
        return -1;
    }
    int ntimes = std::stoi(argv[1]);
    int nworks = std::stoi(argv[2]);
    int nrounds = std::stoi(argv[3]);

    HashBucket::initMemoryPool();
    printf("Benchmark: with memory pool");
    BenchmarkMemoryPool(ntimes, nworks, nrounds);

    printf("Benchmark: without memory pool");
    BenchmarkNew(ntimes, nworks, nrounds);

    return 0;
}