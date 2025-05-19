#include <iostream>
#include <thread>

using namespace std;

int main(int argc, char const *argv[]) {
    auto hello = [] {
        cout << "Hello World from lambda thread." << endl;
    };

    thread t(hello);
    t.join();

    return 0;
}
