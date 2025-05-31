#include <stack>

namespace mgkl {
template <typename T>
class StackedQueue {
public:
    void push(T val);

    T pop();

    bool empty() {
        return ist.empty() && ost.empty();
    }

private:
    std::stack<T> ist;
    std::stack<T> ost;
};
} // namespace mgkl