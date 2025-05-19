#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

void init() {
    cout << "[thread " << this_thread::get_id() << "] Initialing..." << endl;
}

void worker(once_flag *flag, int pid) {
    cout << "pid is " << pid << endl;
    cout << "[thread " << this_thread::get_id() << "] is executing." << endl;
    call_once(*flag, init);
}

int main(int argc, char const *argv[]) {
    once_flag flag;

    thread t1(worker, &flag, 0);
    thread t2(worker, &flag, 1);
    thread t3(worker, &flag, 2);

    t1.join();
    t2.join();
    t3.join();

    return 0;
}
