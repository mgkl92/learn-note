#include <iostream>
#include <thread>
#include <future>
#include <cmath>

using namespace std;

static const int MAX = 10e8;

static double sum = 0;

void worker(int min, int max) {
    for (int i = min; i < max; ++i) {
        sum += sqrt(i);
    }
}

class Worker {
public:
    Worker(int min, int max) : min_(min), max_(max) {}

    double operator()() {
        cout << "Callable task triggered in thread " << this_thread::get_id() << endl;

        for (int i = min_; i < max_; ++i) {
            result_ += sqrt(i);
        }

        return result_;
    }
private:
    int min_;
    int max_;
    double result_;
};

int main(int argc, char const *argv[]) {
    sum = 0;

    // function
    auto f1 = async(launch::async, worker, 0, MAX);
    cout << "Async task triggered" << endl;
    
    // lambda expresion
    double result = 0;
    auto f2 = async(launch::async, [&result](){
        cout << "Lambda task triggered in thread " << this_thread::get_id() << endl;
        for (int i = 0; i < MAX; ++i) {
            result += sqrt(i);
        }
    });

    // callable object
    auto f3 = async(launch::async, Worker(0, MAX));

    f1.wait();
    cout << "Async task finished, result is " << sum << endl;

    f2.wait();
    cout << "Lambda task finished, result is " << sum << endl;

    f3.wait();
    cout << "Callable task finished, result is " << sum << endl;
    return 0;
}
