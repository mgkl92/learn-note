#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>

using namespace std;

static const int MAX = 10e8;
static double sum = 0;

void worker(int min, int max) {
    for (int i = min; i <= max; ++i) {
        sum += sqrt(i);
    }
}

void serial_task() {
    auto start_time = chrono::steady_clock::now();
    sum = 0;
    worker(0, MAX);
    auto end_time = chrono::steady_clock::now();
    auto ms = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();

    cout << "Serail task finish, " << ms << " ms consumed, Result: " << sum << endl;
}

void concurrent_task(int min, int max) {
    auto start_time = chrono::steady_clock::now();

    unsigned concurrent_count = thread::hardware_concurrency();
    cout << "Hardware Concurrency: " << concurrent_count << endl;

    min = 0;
    sum = 0;

    vector<thread> threads;
    for (size_t t = 0; t < concurrent_count; ++t) {
        int range = max / concurrent_count * (t + 1);
        threads.push_back(thread(worker, min, range));
        min = range + 1;
    }

    for (auto &t : threads) {
        t.join();
    }

    auto end_time = chrono::steady_clock::now();
    auto ms = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();

    cout << "Concurrent task finish, " << ms << " ms consumed, Result: " << sum << endl;
}

int main(int argc, char const *argv[]) {
    // serial_task();
    /**
     * Output
     * Serail task finish, 10684031 ms consumed, Result: 2.10819e+13
     */

    concurrent_task(0, MAX);
    /**
     * Output
     * Hardware Concurrency: 4
     * Concurrent task finish, 6337956 ms consumed, Result: 8.16751e+12
     *
     * Note: The result is wrong!
     *
     * The reason is variable `sum` is shared by multiple threads
     * and there is no guarantee that the read/write order of each thread.
     */
    return 0;
}
