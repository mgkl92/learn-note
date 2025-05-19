#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <thread>

using namespace std;

void print_time() {
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);

    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%Y-%m-%d %X");
    cout << "Now is: " << ss.str() << endl;
}

void sleep_thread() {
    this_thread::sleep_for(chrono::seconds(3));
    cout << "[thread " << this_thread::get_id() << "] is waking up" << endl;
}

void loop_thread() {
    for (size_t i = 0; i < 10; ++i) {
        cout << "[thread " << this_thread::get_id() << "] print: " << i << endl;
    }
}

int main(int argc, char const *argv[]) {
    print_time();

    thread t1(sleep_thread);
    thread t2(loop_thread);

    t1.join();
    t2.detach();

    cout << "joinale of t1: " << (t1.joinable() ? "true" : "false") << endl;
    cout << "joinale of t2: " << (t2.joinable() ? "true" : "false") << endl;

    print_time();

    return 0;
}
