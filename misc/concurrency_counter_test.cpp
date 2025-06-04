
// #include <iostream>
// #include <thread>
// #include <mutex>
// #include <atomic>
// #include <vector>
// #include <chrono>

// long long globalCount = 0;

// void ThreadFunction() {
//     for (int i = 0; i < 100000; ++i) {
//         globalCount += 1;
//     }
// }

// int main() {
//     std::vector<std::thread> threads;

//     std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();

//     for (int i = 0; i < 10; ++i) {
//         threads.push_back(std::thread(ThreadFunction));
//     }

//     for (int i = 0; i < 10; ++i) {
//         threads[i].join();
//     }
//     std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();

//     std::cout << "当前总数为:" << globalCount << std::endl;
//     std::cout << "消耗时间为：" << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << "毫秒" << std::endl;

//     return 0;
// }

/**
 * 当前总数为:367566
 * 消耗时间为：3毫秒
 */

// #include <iostream>
// #include <thread>
// #include <mutex>
// #include <atomic>
// #include <vector>
// #include <chrono>

// long long globalCount = 0;
// std::mutex globalMutex;

// void ThreadFunction() {
//     for (int i = 0; i < 100000; ++i) {
//         std::lock_guard<std::mutex> lock(globalMutex);
//         globalCount += 1;
//     }
// }

// int main() {
//     std::vector<std::thread> threads;

//     std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();

//     for (int i = 0; i < 10; ++i) {
//         threads.push_back(std::thread(ThreadFunction));
//     }

//     for (int i = 0; i < 10; ++i) {
//         threads[i].join();
//     }
//     std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();

//     std::cout << "当前总数为:" << globalCount << std::endl;
//     std::cout << "消耗时间为：" << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << "毫秒" << std::endl;
//     return 0;
// }

/**
 * 当前总数为:1000000
 * 消耗时间为：43毫秒
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <chrono>

std::atomic<int> globalCount(0);

void ThreadFunction() {
    for (int i = 0; i < 100000; ++i) {
        globalCount += 1;
    }
}

int main() {
    std::vector<std::thread> threads;

    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();

    for (int i = 0; i < 10; ++i) {
        threads.push_back(std::thread(ThreadFunction));
    }

    for (int i = 0; i < 10; ++i) {
        threads[i].join();
    }
    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();

    std::cout << "当前总数为:" << globalCount << std::endl;
    std::cout << "消耗时间为：" << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << "毫秒" << std::endl;

    return 0;
}

/**
 *当前总数为:1000000
 *消耗时间为：15毫秒
 */

