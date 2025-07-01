// C++20 协程
// Link: https://mp.weixin.qq.com/s/RctE_ApJnLHnmEtnrT7OEw

// #include <thread>
// #include <random>
// #include <functional>
// #include <iostream>
// #include <coroutine>
// #include <memory>

// 2. 协程
// 协程是一个函数，它可以暂停以及恢复执行。
// 普通函数是线程相关的，函数的状态跟线程紧密关联；而协程是线程无关的，它的状态与任何线程都没有关系。
// 协程的状态是保存在堆内存上的。

// 3. 协程优点
// 优化异步逻辑代码

// ``BeginRead()`` 开启了一个新的线程来生成一个随机的整数，模拟异步操作。
// class IntReader {
// public:
//     void BeginRead() {
//         std::thread thread([]() {
//             std::srand(static_cast<unsigned int>(std::time(nullptr)));
//             int value = std::rand();
//             // 使用 value ...        });
//         });

//         thread.detach();
//     }
// };

// 使用回调函数获取操作结果
// class IntReader {
// public:
//     void BeginRead(const std::function<void(int)> &callback) {
//         std::thread t([callback]() {
//             std::srand(static_cast<unsigned int>(std::time(nullptr)));
//             int value = std::rand();

//             callback(value);
//         });

//         t.detach();
//     }
// };
// void PrintInt() {
//     IntReader reader;
//     reader.BeginRead([](int result) {
//         std::cout << "result = " << result << std::endl;
//     });
// }

// "回调地狱"
// void PrintInt() {
//     IntReader reader1;
//     reader1.BeginRead([](int result1) {
//         int total = result1;

//         IntReader reader2;
//         reader2.BeginRead([&](int result2) {
//             total += result2;

//             IntReader reader3;
//             reader3.BeginRead([&](int result3) {
//                 total += result3;

//                 std::cout << "total = " << std::endl;
//             });
//         });
//     });
// }

// 4. 实现协程
// class IntReader {
// public:
//     // 协程在执行 co_await 的时候，会先调用 await_ready()来询问“操作是否已完成”，
//     // 如果函数返回了 true ，协程就不会暂停，而是继续往下执行。
//     bool await_ready() {
//         return false;
//     }

//     // 该函数的目的是用来接收协程句柄（也就是std::coroutine_handle<> 参数），
//     // 并在异步操作完成的时候通过这个句柄让协程恢复执行.
//     void await_suspend(std::coroutine_handle<> handle) {
//         std::thread t([this, handle]() {
//             std::srand(static_cast<unsigned int>(std::time(nullptr)));
//             int value = std::rand();

//             value_ = std::rand();

//             handle.resume();
//         });

//         t.detach();
//     }

//     // 它的返回值就是 co_await 操作符的返回值,
//     // 当协程恢复执行，或者不需要暂停的时候，会调用这个函数。
//     int await_resume() {
//         return value_;
//     }

// private:
//     int value_ = 0;
// };

// 4.2 预定义的 Awaitable 类型
// C++预定义了个符合 Awaitable 规范的类型: ``std::suspend_never``(不暂停) 和 ``std::suspend_always``(要暂停)。
// 区别仅在于 await_ready()函数的返回值: ``std::suspend_never`` 会返回 true，而 ``std::suspend_always`` 会返回 false。
// class Task {
// public:
//     // 4.3 协程返回类型喝 promise_type
//     // C++对协程的返回类型只有一个要求：包含名为 promise_type 的内嵌类型。
//     class promise_type {
//     public:
//         // 用来创建协程的返回值
//         Task get_return_object() {
//             return {};
//         }

//         std::suspend_never initial_suspend() {
//             return {};
//         }

//         std::suspend_never final_suspend() noexcept {
//             return {};
//         }

//         void unhandled_exception() {
//         }

//         void return_void() {
//         }
//     };
// };

// Task PrintInt() {
//     IntReader reader1;
//     int total = co_await reader1;

//     IntReader reader2;
//     total += co_await reader2;

//     IntReader reader3;
//     total += co_await reader3;

//     std::cout << "total = " << total << std::endl;
// }

// 4.4 co_return
// 使用 co_return 操作符可以从协程中返回数据
// class IntReader {
// public:
//     bool await_ready() {
//         return false;
//     }

//     void await_suspend(std::coroutine_handle<> handle) {
//         std::thread t([this, handle]() {
//             std::srand(static_cast<unsigned int>(std::time(nullptr)));
//             value_ = std::rand();

//             handle.resume();
//         });
//     }

//     int await_resume() {
//         return value_;
//     }

// private:
//     int value_ = 0;
// };

// class Task {
// public:
//     class promise_type {
//     public:
//         promise_type() :
//             value_(std::make_shared<int>()) {
//         }

//         Task get_return_object() {
//             return Task{value_};
//         }

//         void return_value(int value) {
//             *value_ = value;
//         }

//         std::suspend_never initial_suspend() {
//             return {};
//         }

//         std::suspend_never final_suspend() noexcept {
//             return {};
//         }

//         void unhandled_exception() {
//         }

//     private:
//         std::shared_ptr<int> value_;
//     };

// public:
//     Task(const std::shared_ptr<int> &value) :
//         value_(value) {
//     }

//     int GetValue() const {
//         return *value_;
//     }

// private:
//     std::shared_ptr<int> value_;
// };

// Task GetInt() {
//     IntReader reader1;
//     int total = co_await reader1;

//     IntReader reader2;
//     total += co_await reader2;

//     IntReader reader3;
//     total += co_await reader3;
//     // ``co_return total`` 这个表达式等价于 ``promise_type.return_value(total)``,
//     // 也就是说，返回的数据会通过 ``return_value()`` 函数传递给 ``promise_type`` 对象,
//     // ``promise_type`` 要实现这个函数才能接收到数据。
//     // 当协程调用了 ``co_return`` ，意味着协程结束了，与这个协程实例有关的内存都会被释放掉。
//     co_return total;
// }

// 4.5 co_yield
// ``co_yield`` 的作用是，返回一个数据，并且让协程暂停，然后等下一次机会恢复执行。
// class IntReader {
// public:
//     bool await_ready() {
//         return false;
//     }

//     void await_suspend(std::coroutine_handle<> handle) {
//         std::thread t([this, handle]() {
//             static int seed = 0;
//             value_ = ++seed;

//             handle.resume();
//         });

//         t.detach();
//     }

//     int await_resume() {
//         return value_;
//     }

// private:
//     int value_ = 0;
// };

// class Task {
// public:
//     class promise_type {
//     public:
//         Task get_return_object() {
// // ``std::coroutine_handle的from_promise()`` 函数可以通过 ``promise_type`` 对象获取与之关联的协程句柄。
// return std::coroutine_handle<promise_type>::from_promise(*this);
//         }

//         std::suspend_always yield_value(int value) {
//             value_ = value;
//             return {};
//         }

//         void return_void() {
//         }

//         std::suspend_never initial_suspend() {
//             return {};
//         }

//         std::suspend_never final_suspend() noexcept {
//             return {};
//         }

//         void unhandled_exception() {
//         }

//         int GetValue() const {
//             return value_;
//         }

//     private:
//         int value_ = 0;
//     };

// public:
//     Task(std::coroutine_handle<promise_type> handle) :
//         coroutine_handle_(handle) {
//     }

//     int GetValue() const {
//         return coroutine_handle_.promise().GetValue();
//     }

//     void Next() {
//         coroutine_handle_.resume();
//     }

// private:
//     std::coroutine_handle<promise_type> coroutine_handle_;
// };

// Task GetInt() {
//     while (true) {
//         IntReader reader;
//         int value = co_await reader;
//         // ``co_yield value`` 这个表达式等价于 ``co_await promise_type.yield_value(value)``
//         co_yield value;
//     }
// }

// int main(int argc, char const *argv[]) {
//     auto task = GetInt();

//     std::string line;
//     while (std::cin >> line) {
//         std::cout << "value = " << task.GetValue() << std::endl;
//         task.Next();
//     }

//     return 0;
// }

// 4.6 协程生命周期
// 修改 ``promise_type`` 中 ``final_supsend()`` 函数的返回类型，从 ``std::suspend_never`` 改成 ``std::suspend_always``。
// 协程在结束的时候，会调用 ``final_suspend()`` 来决定是否暂停，如果这个函数返回了要暂停，那么协程不会自动释放，此时协程句柄还是有效的，可以安全访问它内部的数据。
// 只要协程处于暂停状态，就可以调用协程句柄的 ``destroy()`` 函数来释放它。

// 4.7 异常处理
// try {
//     co_await promise_type.initial_suspend();

//     ...
// } catch (...) {
//      // 异常处理
//      promise_type.unhandled_exception();
// }

// co_await promise_type.final_supsend();