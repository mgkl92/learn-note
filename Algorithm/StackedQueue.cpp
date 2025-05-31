#include "StackedQueue.h"
#include <stdexcept>
namespace mgkl {
template <typename T>
void StackedQueue<T>::push(T val) {
    ist.push(T);
}

template <typename T>
T StackedQueue<T>::pop() {
    if (ost.empty() && !ist.empty()) {
        while (!ist.empty()) {
            T temp = ist.top();
            ist.pop();

            ost.push(temp);
        }
        return ost.pop();
    }

    throw std::runtime_error("Queue is empty.");
}
} // namespace mgkl

template<class T>
struct Test{

};