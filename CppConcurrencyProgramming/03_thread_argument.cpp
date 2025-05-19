#include <iostream>
#include <thread>

using namespace std;

void hello(string name) {
    cout << "Hello " << name << endl;
}

int main(int argc, char const *argv[]) {
    thread t(hello, "mgkl");
    t.join();

    return 0;
}
