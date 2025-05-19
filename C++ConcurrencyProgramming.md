# 2 线程管理

## 线程管理

**启动线程**

```cpp
// 函数
void do_some_work();
std::thread my_thread(do_some_work);

// 可调用对象
class background_task {
    public:
        void operator()() const {
            ...
        }
};
backgroud_task bt;
std::thread my_thread(bt);

// 错误：函数声明
std::thread my_thread(background_task());
// 解决：
std::thread my_thread((background_task())); // 多组括号 
std::thread my_thread{background_task()}; // 列表初始化
// lambda 表达式
std::thread my_thread([] {
    ...
});
```

你需要在启动线程后，确定等待线程结束（加入式）或令其自主运行（分离式）。

如果在 std::thread 对象销毁之前未确定，程序将终止运行，std::thread 析构函数会调用 std::terminate()。


**分离式进程**

此外，如果采取分离式进程，我们需要保证在线程结束前其访问数据的有效性。

```cpp
struct func {
    int &i;
    func(int & i_) : i (i_) {}
    
    void operator()() {
        for (unsigned j = 0; j < 1000000; ++j) {
            do_something(i); // 潜在访问隐患：悬空引用
        }
    }
};

void oops() {
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread my_thread(my_func);
    my_thread.detach(); // 可能 oops() 执行结束，线程仍在运行
}
```

*解决方法：将数据复制到线程中，而非复制到共享数据中。*

需要注意的是，当主线程调用 detach() 对子线程分离后，子线程将在后台运行，主线程不再持有对子线程的引用！

**加入式进程**

*上述问题的解决，除了将数据复制到线程中，还可以令执行 oops() 函数的线程等待 执行 func 可调用式对象线程的结束，即使用 join() 方法。*

**异常情况**

我们需要确保在启动线程之后，调用 join() 函数之前，主线程的执行不会发生异常；否则，这将导致 join() 函数的调用被跳过！

```cpp
// 1 使用 try/catch 进行异常处理
struct func;

void f() {
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread t(my_func);

    try {
        do_something_in_current_thread();
    } catch (...) {
        t.join();
        throw;
    }
    t.join();
}

// 2 使用 RAII 等待线程完成
class thread_guard {
    std::thread &t;
    
    public:
        explicit thread_guard(std::thread& t_): t(t_) {}
        ~thread_guard() {
            if(t.joinable()) {
                t.join();
            }
        }

        thread_guard(thread_guard const &) = delete;
        thread_guard& operator=(thread_guard const&) = delete;
}

void f() {
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread t(my_func);
    thread_guard tg(t);

    do_something_in_current_thread();
}
```

## 参数传递

```cpp
void f(int i, std::string const &s);
std::thread t(f, 3, "hello");
```

在线程的上下文，字面量 hello 将被转化为 std::string 对象。

但值得注意当指向动态变量的指针作为参数传递给线程时，情况将发生改变。

```cpp
void f(int i, std::string const& s);

void oops(int some_param) {
    char buffer[1024];
    sprintf(buffer, "%i", some_param);
    std::thread t(f, 3, buffer); 
    t.detach();
}
```

上述代码的问题在于，oop() 可能会在字面值转化成 std::string 对象之前发生崩溃，此时，线程只是复制了没有转化成期望类型的字符串字面量。

解决方法如下，即使用显式类型转换。

```cpp
    std::thread t(f, 3, std::string(buffer));
```

## 所有权转移

```cpp
void some_func();
void some_other_func();

std::thread t1(some_func);
std::thread t2 = std::move(t1); // t1 -> t2

t1 = std::thread(some_other_func); // 移动赋值操作

std::thread t3; // 默认方式构造线程
t3 = std::move(t2); // t2 -> t3

t1 = std::move(t3); // t1 已经绑定线程，再次绑定使得系统调用 std::terminate() 终止程序！
```

**std::thread 支持移动，这使得线程的所有权可在函数外进行转移**。

```cpp
std::thread f() {
    void some_func();
    return std::thread(some_func);
}

std::thread g(int some_param) {
    void some_other_func(int);
    std::thread t(some_other_func, some_param);

    return t;
}
```

**作为参数传递**
```cpp
void f(std::thread t);

void g() {
    void some_func;
    f(std::thread(some_func));

    std::thread(some_func);
    f(std::move(t));
}
```

**改进使用 RAII 等待线程完成**

```cpp
class scoped_thread
{
    std::thread t; // 直接使用 std::thread 实例，而非引用
    
    public:
        explicit scoped_thread(std::thread t_) : t(std::move(t_)) {
            if (!t.joinable()) {
                throw std::logic_error("No thread");
            }
        }
        ~scoped_thread() {
            t.join();
        }

        scoped_thread(scoped_thread const &) = delete;
        scoped_thread& operator=(scoped_thread const &) = delete;
};

struct func;

void f() {
    int some_local_state;
    scoped_thread t(std::thread(func(some_local_state)));
    do_something_in_current_thread();
}
```

## 动态决定线程数量

std::thread::hardware_concurrency() 函数用于返回能够同时并发在一个程序中的线程数量。

```cpp
template<typename Iterator, typename T>
struct accumulate_block {
    void operator()(Iterator first, Iterator last, T& result) {
        result = std::accumulate(first, last, result);
    }
};

template<typename Iterator, typaname T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    unsigned long const length = std::distance(first, last);

    if (!length) {
        return init;
    }

    unsigned long const min_per_thread = 25;
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;

    unsigned long const hardware_threads = std::thread::hardware_concurrency();

    unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads :2 , max_threads);

    std::vector<T> results(num_threads);
    std::vector<std::thread> threads(num_threads - 1);

    Iterator block_start = first;
    for (unsigned long i = 0; i < num_threads - 1; ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        threads[i] = std::thread(accumulate_block<Iterator>(), block_start, block_end, std::ref(results[i]));

        block_start = block_end;
    }

    accumulate_block<Iterator, T>() (block_start, last, results[num_threads - 1]);
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

    return std::accumulate(results.begin(), results.end(), init);
}
```

## 识别线程

线程标识类型为 std::thread::id，你可以使用 std::thread 对象的成员函数 get_id() 或使用 std::this_thread::get_id() 来获取当前线程的标识。

如果没有绑定任何线程，get_id() 将返回 std::thread::type 的默认构造值，该值表示没有任何线程。

线程标识能够作为关联容器类的键值。

```cpp
// 使用线程标识进行流程控制
std::thread::id master_thread;

void some_core_part_of_algorithm() {
    if (std::this_thread::get_id() == master_thread) {
        do_master_thread_work();
    }
    do_common_work();
}
```

# 3 线程间共享数据

## 共享数据带来的问题

**如何理解不变量？**

在双链表中，每个结点都有指向列表中下一个结点和指向前一个结点的指针。

在删除列表中结点时，两侧指针均需更新，但仅当一边更新完成时，不变量被破坏；直到另外一侧也完成更新，不变量恢复稳定。

**条件竞争**

以电影购票为例，你的座位选择范围取决于之前已经预定的作为以及与其他收银台座位售卖的相对顺序。

在并发中竞争条件的形成，取决于一个以上线程的相对顺序。但在大多数情况下，即使改变执行顺序，其结果也是可以接受的，这是一种良性的竞争。

*数据竞争是一种特殊的条件竞争，也即多个并发的线程去修改同一个独立的对象。**恶性竞争通常发生于需要完成对多于一个数据块的修改。***

## 使用互斥量保护共享数据

C++ 中通过实例化 std::mutex 创建互斥量，通过调用成员函数 lock() 进行上锁以及 unlock() 进行解锁。

C++ 标准库提供了 RAII 的模板类 std::lock_guard, 在其构造时提供已锁的互斥量并在析构时进行解锁。

```cpp
#include <list>
#include <mutex>
#include <algorithm>

std::list<int> some_list;
std::mutex some_mutex;

void add_to_list(int new_value) {
    std::lock_guard<std::mutex> guard(some_mutex);
    some_list.push_back(new_value);
}

bool list_contains(int value_to_find) {
    std::lock_guard<std::mutex> guard(some_mutex);
    return std::find(some_list.begin(), some_list.end(), value_to_find) != some_list.end();
}
```

**不要将保护数据作为运行时参数**

```cpp
class some_data {
    int a;
    std::string b;

    public:
        void do_something() {}
};

class data_wrapper{
    private:
        some_data data;
        std::mutex m;

    public:
        template<typename Function>
        void process_data(Function func) {
            std::lock_guard<std::mutex> l(m);
            func(data);
        }
};

some_data * unprotected;

void malicious_function(some_data & protected_data) {
    unprotected = &protected_data;
}

data_wrapper x;
void foo() {
    x.process_data(malicious_function);
    unprotected->do_something();    // 无保护情况下访问保护数据
}
```

**接口内在的条件竞争**

```cpp
template<typename T, typename Container=std::deque<T>>
class stack {
    public:
        explicit stack(const Container&);
        explicit stack(const&& = Container());
        template <class Alloc> explicit stack(const Alloc &);
        template <class Alloc> stack(const Container&, const Alloc&);
        template <class Alloc> stack(Container &&, const Alloc &);
        template <class Alloc> stack(stack &&, const Alloc&);

        bool empty() const;
        size_t size() const;
        T& top();
        T const & top() const;
        void push(T const &);
        void push(T &&);
        void pop();
        void swap(stack&&);
};
```

如果 stack 的实例是非共享的，使用 empty() 检查再调用 top() 访问栈顶元素是安全的。

```cpp
stack<int> st;
if (!s.empty()) {   // 1
    int const value = s.top();  // 2
    s.pop();    // 3

    do_something(value);
}
```

如果 stack 的实例是共享的，上述调用顺序将不再安全；因为，在调用 empty() 和 pop() 之间，可能来自其他线程的 pop() 调用删除了最后一个元素。此外，函数 top() 与 pop() 之间也存在条件竞争。

**线程安全的堆栈**

```cpp
#include <exception>
#include <memory>

struct empty_stack : std::exception {
    const char * what() const throw() {
        return "empty stack!";
    };
};

template<typename T>
class threadsafe_stack {
    private:
        std::stack<T> data;
        mutable std::mutex m;

    public:
        threadsafe_stack() : data(std::stack<T>()) { }

        threadsafe_stack(const threadsafe_stack& other) {
            std::lock_guard(std::mutex) lock(other.m);
            data = other.data;
        }

        threadsafe_stack& operator=(const threadsafe_stack &) = delette;

        void push(T new_value) {
            std::lock_guard<std::mutex> lock(m);
            data.push(new_value);
        }

        std::shared_ptr<T> pop() {
            std::lock_guard<std::mutex> lock(m);

            if (data.empty()) {
                throw empty_stack();
            }

            std::shared_ptr<T> const res(std::make_shared<T>(data.top()));
            data.pop();

            return res;

        }

        void pop(T& value) {
            std::lock_guard<std::mutex> lock(m);
            if (data.empty()) {
                throw empty_stack();
            }

            value = data.top();
            data.pop();
        }

        bool empty() const {
            std::lock_guard<std::mutex> lock(m);
            return data.empty();
        }
};
```

**死锁问题**

```cpp
class some_big_object;
void swap(some_big_object &lhs, some_big_object &rhs);
class X {
    private:
        some_big_object some_detail;
        std::mutex m;
    public:
        X(some_big_object const& sd) : some_detail(sd) {}

        friend void swap(X& lhs, X& rhs) {
            if (&rhs == &rhs) {
                return ;
            }

            std::lock(lhs.m, rhs.m);
            std::lock_guard<std::mutex> lock_a(lhs.m, std::adopt_lock);
            std::lock_guard<std::mutex> lock_b(rhs.m, std::adopt_lock);
            swap(lhs.some_detail, rhs.some_detail);
        }
};
```

**避免死锁**

- 避免嵌套锁

- 避免在持有锁时调用用户提供的代码

- 使用固定顺序获得锁

- 使用锁的层次结构

*锁的层次意义在于提供对运行时约定是否被坚持的检查。*

当代码试图对一个互斥量上锁时，如果该层锁已经被底层持有时，上锁是不允许的。

```cpp
hierachical_mutex high_level_mutex(10000);
hierachical_mutex low_level_mutex(5000);

int do_low_level_stuff();

int low_level_func() {
    std::lock_guard<hierachical_mutex> lock(low_level_mutex);
    return do_low_level_stuff();
}

void high_level_stuff(int some_param);

void high_level_func () {
    std::lock_guard<hierachical_mutex> lock(high_level_mutex);
    high_level_stuff(low_level_func());
}

// 上锁成功：high_level_mutex 层级高于 low_level_mutex
void thread_a() {
    high_level_func();
    do_other_stuff();
}

hierarchical_mutex other_mutex(100);

void do_other_stuff();

void other_stuff() {
    high_level_func();
    do_other_stuff();
}

// 上锁失败：other_mutex 锁的层级低于 high_level_mutex
void thread_b() {
    std::lock_guard<hierarchical_mutex> lock(other_mutex);
    other_stuff();
}

**层级互斥量的实现**

```cpp
class hierarchical_mutex {
    std::mutex inernal_mutex;

    unsigned long const hierarchy_value;
    unsigned long previous_hierachy_value;

    // 当前线程层级
    static thread_local unsigned long this_thread_hierachy_value;

    void check_for_hierachy_violation() {
        if (this_thread_hierarchy_value <= hierarchy_value) {
            throw std::logic_error("mutex hierachy violated");
        }
    }

    void update_hierarchy_value() {
        previous_hierarchy_value = this_thread_hierarchy_value;
        this_thread_hierarchy_value = hierarchy_value;
    }

    public:
        explicit hierarchical_mutex(unsigned long value):
            hierarchy_value(value),
            previous_hierarchy_value(0) { }
        
        void lock() {
            check_for_hierarchy_violation();
            internal_mutex.lock();
            update_hierarchy_value();
        }

        void unlock() {
            this_thread_hierarchy_value = previous_hierarchy_value;
            internal_mutex.unlock();
        }

        bool try_lock() {
            check_for_hierarchy_violation();
            if (!internal_mutex.try_lock()) {
                return false;
            }
            update_hierarchy_value();
            return true;
        }
}

// 当前线程的层级值被设置为最大值，这保证了最初所有线程都能够上锁
thread_local unsigned long hierarchical_mutex::this_thread_hierachy_value(ULONG_MAX);
```

**灵活的锁：std::unique_lock**

std::unique_lock 相比 std::lock_guard 会占用较多空间且效率稍慢，这就是使得锁灵活的代价。

```cpp
class some_big_object;
void swap(some_big_object& lhs, some_big_object& rhs);
class X {
    private: 
        some_big_object some_detail;
        std::mutex m;
    public:
        X (some_big_object const & sd):
            some_detail(sd) {}

        friend void swap(X& lhs, X& rhs) {
            if (&lhs == &rhs) {
                return ;
            }

            std::unique_lock<std::mutex> lock_a(lhs.m, std::defer_lock);
            std::unique_lock<std::mutex> lock_b(rhs.m, std::defer_lock);
            std::lock(lock_a, lock_b);
            swap(lhs.some_detail, rhs.some_detail);
        }
};
```

**互斥量所有权的传递**

```cpp
std::unique_lock<std::mutex> get_lock() {
    extern std::mutex some_mutex;
    std::unique_lock<std::mutex> lock(some_mutex);
    prepare_data();
    return lock;
}

void process_data() {
    std::unique_lock<std::mutex> lock(get_lock());
    do_something();
}
```

**锁的粒度**

- [ ] TODO

## 其他保护共享数据的方法

**保护共享数据的初始化过程**

```cpp
// 延迟初始化
std::shared_ptr<some_resource> resource_ptr;
void foo() {
    if (!resource_ptr) {
        resource_ptr.reset(new some_resource); // 临界区
    }

    resource_ptr->do_something();
}


// 线程安全的延迟初始化
std::shared_ptr<some_resource> resource_ptr;
std::mutex resource_mutex;

void foo() {
    std::unique_lock<std::mutex> lock(resource_mutex);
    if (!resource_ptr) {
        resource_ptr.reset(new some_resource);
    }

    lock.unlock();
    resource_ptr->do_something();
}

// 双重检查锁
// 1 和 3 处存在条件竞争
// 可能存在线程 resource_ptr 不为空但尚未看到 some_resource 的实例，从而导致资源使用 4 产生未定义行为
void undefined_behaviour_with_double_checked_locking() {
    if (!resource_ptr) { // 1
        std::lock_guard<std::mutex> lock(resource_mutex);
        if (!resource_ptr) { // 2
            resource_ptr.reset(new some_resource); // 3
        }
    }

    resource_ptr->do_something(); // 4
}

// 使用 std::once_flag 和 std::call_once
std::shared_ptr<some_resource> resource_ptr;
std::once_flag resource_flag;

void init_resource() {
    resoure_ptr.reset(new some_resource);
}

void foo() {
    std::call_once(resource_flag, init_resource);
    resource_ptr->do_something();
}
```

**使用 std::call_once 实现线程安全的类成员的延迟初始化**

```cpp
class X {
    private:
        connection_info connection_details;
        connection_handle connection;
        std::once_flag connection_init_flag;

        void open_connection() {
            connection = connection_manager.open(connection_details);
        }

    public:
        X (connection_info const & connection_details_):
            connection_details(connection_details) {}
        
        void send_data(data_packet const &data) {
            std::call_once(connection_init_flag, &X::open_connection, this);
            connection.send_data(data);
        }

        data_packet receive_data() {
            std::call_once(connection_init_flag, &X::open_connection, this);
            return connection.receive_data();
        }
}
```

**局部静态变量的初始化**

在多线程情况中，每个线程都认为自己是第一个初始化这个变量的线程或一个线程对变量进行初始化，而另外一个线程要使用该变量但初始化过程还未完成。

在 C++ 11 以后，初始化及定义完全在一个线程中发生，没有任何其他线程可以在初始化完成前对齐进行处理！

```cpp
class my_class;
my_class& get_my_class_instance() {
    static my_class instance;
    return instance;
}
```

**保护很少更新的数据结构**

```cpp
#include <map>
#include <string>
#include <mutex>
#include <boost/thread/shared_mutex.hpp>

class dns_entry;

class dns_cache {
    std::map<std::string, dns_enry> entries;
    mutable boost::shared_mutex entry_mutex;

    public:
        dns_entry find_encry(std::string const & domain) const {
            boost::shared_lock<boost::shared_mutex> lock(entry_mutex);
            std::map<std::string, dns_entry>::const_iterator const it = entries.find(domian);

            return (it == entries.end()) ? dns_entry() : it->second;
        }

        void update_or_add_entry(std::string const& domain, dns_entry const & dns_details) {
            std::lock_guard<boost::shared_mutex> lock(entry_mutex);
            entries[domain] = dns_details;
        }
};
```

**嵌套锁**

- [ ] TODO

# 4 同步并发操作

## 等待一个事件或其他条件

**使用 std::this_thread::sleep_for() 进行周期性的间歇**

```cpp
bool flag;
std::mutex m;

void wait_for_flag() {
    std::unique_lock<std::mutex> lk(m);
    while (!flag) {
        lk.unlock();
        std::this_thread::sleep_for(std::chrono::millseconds(100));
        lk.lock();
    }
}
```

上述问题在于如何选择合适的等待间隙，如太短的休眠时间和没休眠一样都会浪费得太多的执行时间，而太长的休眠时间可能会使得任务等待线程醒来。

**使用 std::condition_variable 等待条件达成**

```cpp
std::mutex m;
std::queue<data_chunk> data_queue;
std::condition_varibale data_cond;

void data_preparation_thread() {
    while (more_data_to_prepare()) {
        const data_chunk data = prepare_data();
        std::lock_guard<std::mutex> lk(m);
        data_queue.push(data);
        data_cond.notify_one();
    }
}

void data_processing_thread() {
    while (true) {
        std::unique_lock<std::mutex> lk(m);
        data_cond.wait(lk, [] { return !data_queue.empty(); });
        data_chunk data = data_queue.front();
        data_queue.pop();
        lk.unlock();

        process(data);
        if(is_last_chunk(data)) {
            break;
        }
    }
}
```

**使用条件变量构建线程安全队列**

```cpp
// std::queue API
template <class T, class Container = std::deque<T>>
class queue {
    public:
        explicit queue(const Container&);
        explicit queue(const Container&&);
        template <class Alloc> explicit queue(const Alloc&);
        template <class Alloc> queue(const Container&, const Alloc&);
        template <class Alloc> queue(const Container &&, const Alloc&);

        void swap(queue& q);

        bool empty() const;
        size_type size() const;

        T& front();
        const T& front() const;

        T& back()
        const T& back() const;

        void push(const T& x);
        void push(T&& x);

        void pop();

        template <class... Args> void emplace(Args&&... args);
};

template <typename T>
class threadsafe_queue {
    public:
        threadsafe_queue() {}

        threadsafe_queue(const threadsafe_queue&) {
            std::lock_guard<std::mutex> lk(other.m);
            data_queue = other.data_queue;
        }

        threadsafe_queue& operator=(const threadsafe_queue&) = delete;

        void push(T new_value) {
            std::lock_gurad<std::mutex> lk(m);
            data_queue.push(new_value);
            data_cond.notify_one();
        }

        bool try_pop(T& value) {
            std::lock_guard<std::mutex> lk(m);
            if (data_queue.empty()) {
                return false;
            }

            value = data_queue.front();
            data_queue.pop();
            return true;
        }

        std::shared_ptr<T> try_pop() {
            std::lock_guard<std::mutex> lk(m);
            if (data.empty()) {
                return false;
            }

            std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
            data_queue.pop();

            return res;
        }

        void wait_and_pop(T& value) {
            std::unique_lock<std::mutex> lk(m);
            data_cond.wait(lk, [this]{ return !data_queue.empty(); });

            value = data_queue.front();
            data_queue.pop();
        }

        std::shared_ptr<T> wait_and_pop() {
            std::unique_lock<std::mutex> lk(m);
            data_cond.wait(lk, [this] { return !data_queue.empty(); });

            std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
            data_queue.pop();

            return res;
        }

        bool empty() const {
            std::lock_guard<std::mutex> lk(m);
            return data_queue.empty();
        }

    private: 
        std::queue<T> data_queue;

        mutable std::mutex m;
        std::condition_varibale data_cond;
};

threadsafe_queue<data_chunk> data_queue;

void data_preparation_thread() {
    while (more_data_to_prepare()) {
        const data_chunk data = prepare_data();
        data_queue.push(data);
    }
}

void data_processing_thread() {
    while(true) {
        data_chunk data;
        data_queue.wait_and_pop(data);
        process(data);
        if (is_last_chunk(data)) {
            break;
        }
    }
}
```

## 使用期望等待一次性事件

在 C++ 标准库中，有两种期望：唯一期望 std::future 和共享期望 std::shared_future。

std::future 的实例只能与一个指定事件关联，而 std::shared_future 的实例允许与多个事件关联。

期望对象本身并不提供同步访问，当多个线程需要访问一个独立的期望对象时，必须使用互斥量或其他同步机制进行保护。

**带返回值的后台任务**

```cpp
#include <future>
#include <iostream>

int find_the_answer_to_ltuae();
void do_other_stuff();

int main() {
    std::future<int> the_answer = std::async(find_the_answer_to_ltuae);
    do_other_stuff();

    std::cout << "The answer is " << the_answer.get() << std::end;
}

// 向函数传递参数
#include <string>
#include <future>

struct X {
    void foo(int, const std::string &);
    std::string bar(const std::string &); 
};
X x;

// 调用 p->foo(42, "hello"), p 为指向 x 的指针
auto f1 = std::async(&X::foo, &x, 42, "hello");

// 调用 tempx.bar("goodbye"), tempx 为 x 的拷贝副本
auto f2 = std::async(&X::bar, x, "goodbye");

// 向可调用对象传递参数
struct Y {
    double operator()(double);
};

Y y;

// 调用 tempy(3.141), tempy 为通过 Y() 移动构造
auto f3 = std::async(Y(), 3.141)

// 调用 y(2.718)
auto f4 = std::async(std::ref(y), 2.718);

// 传递对象参数
X baz(X&);

// 调用 baz(x)
std::async(baz, std::ref(x));

class move_only {
    public:
        move_only();

        move_only(move_only &&);
        move_only(move_only const &) = delete;

        move_only& operator=(move_only &&);
        move_only& operator=(move_only &) = delete;

        void operator()();
};

// 调用 temp(), tem 通过移动构造得到
auto f5 = std::async(move_only());
```

**期望的执行时机**

在默认情况下，期望是否进行等待取决于 std::async 是否启动一个线程，或是否有任务正在进行同步。

```cpp
// 在新线程上执行
auto f6 = std::async(std::launch::async, Y(), 1.2);

// 在 wait() 或 get() 调用时执行
auto f7 = std::async(std::launch::deferred, baz, std::ref(x));

// 选择
auto f8 = std::async(
    std::launch::deferred | std::launch::async,
    baz, std::ref(x));

auto f9 = std::async(baz, std::ref(x));

// 调用延迟函数
f7.wait();
```

**任务与期望**

std::packaged_task 对于一个函数或可调用对象绑定一个期望。当 std::packaged_task 对象被调用时，它就会调用相关函数或可调用对象，并将期望状态设置为就绪，返回值也会被存储为相关数据。

```cpp
template<>
class packaged_task<std::string(std::vector<char> *, int)> {
    public:
        template<typename Callable>
        explicit packaged_task(Callable&& f);
        std::future<std::string> get_future();
        void operator() (std::vector<char>*, int);
}
```

```cpp
// 线程间传递任务
#include <deque>
#include <mutex>
#include <future>
#include <thread>
#include <utility>

std::mutex m;
std::deque<std::packaged_task<void()>> tasks;

bool gui_shutdown_message_received();
void get_and_process_gui_message();

void gui_thread() {
    while (!gui_shutdonw_message_received()) {
        get_and_process_gui_message();
        std::packaged_task<void()> task;
        {
            std::lock_guard<std::mutex> lk(m);
            if (tasks.empty()) {
                continue;
            }

            task = std::move(task.front());
            tasks.pop_front();
        }

        task();
    }
}

std::thread gui_bg_thread(gui_thread);

template<typename Func>
std::future<void> post_task_for_gui_thread(Func f) {
    std::packaged_task<void)_> task(f);
    std::future<void> res = task.get_future();

    std::lock_guard<std::mutex> lk(m);
    task.push_back(std::move(task));

    return res;
}
```

**使用 std::promises**

```cpp
// 使用承诺解决单线程多连接问题
#include <future>

void proccess_connections(connection_set& connections) {
    while (!done(connections)) {
        for(auto connection = connections.begin(), end = connections.end();
            connection != end;
            ++connection) {
                // 接受数据
                if (connection->has_incoming_data()) {
                    data_packet data = connection->incoming();

                    std::promise<payload_type> & p = conncetion->get_promise(data.id);
                    p.set_value(data.payload);
                }

                if (connection->has_outgoing_data()) {
                    outgoing_packet data = connection->top_of_outgoing_queue();
                    connection->send(data.payload);
                    data.promise.set_value(true);
                }
            }
    }
}
```

**为期望存储异常**

```cpp
double square_root(double x) {
    if (x < 0) {
        throw std::out_of_range("x < 0");
    }

    return sqrt(x);
}

// 异常会被存储到期望的结果数据中， 随后期望的状态被设置为就绪
// 调用期望的 get() 方法会抛出存储的异常
std::futre<double> f = std::async(sqaure_root, -1);
double result = f.get();


// 显式调用存储异常

extern std::promise<double> some_promise;
try {
    some_promise.set_value(calculate_value());
} cathc (...) {
    // 使用 std::current_exception() 来检索抛出的异常
    some_promise.set_exception(std::current_exception());
}
```

- [ ] TODO

**多个线程的等待**

std::future 可以处理在线程间数据转移的必要同步，但当调用某一特殊的 std::future 对象的成员函数时，就会让这个线程的数据与其他线程的数据不同步。这是因为 std::future 模型独享同步结果的所有权并且通过调用 get() 方法一次性获取数据。

然而，std::shared_future 可以帮助我们解决上述问题。

std::future 只是可移动的，其所有权可以在不同的实例中相互传递，但仅有一个实例可以获得特定的同步结果；而 std::shared_future 实例时可拷贝的，所以多个对象可以引用同一关联期望的结果！

```cpp
std::promise<int> p;
std::future<int> f(p.get_future());

// 期望 f 合法
assert(f.valid());
std::shared_future<int> sf(std::move(f));

// 期望 f 不合法
assert(!f.valid());

// 共享期望 sf 合法
assert(sf.valid());

```

## 限定等待时间

超时方式：时延和绝对。时延超时需要指定一段时间，如 30 毫秒；而绝对超时需要指定一个时间点。


**时钟**

对于 C++ 标准库而言，时钟就是时间信息源。

时钟是一个类，提供四种不同的信息：

- 现在时间

时钟的当前时间可以通过调用静态成员函数 now() 来获取，如 std::chrono::system_clock::now() 返回系统时钟的当前时间。

- 时间类型

- 时钟节拍

时钟节拍被指定为 1 / x 秒，其中 x 在不同硬件上的取值不同，这由时间周期决定。
例如一个时钟一秒有 25 个节拍，因此一个周期为 std::ratio<1, 25>；而当一个时钟的时钟节拍为每 2.5 秒一次，其周期可表示为 std::ration<5, 2>。

- 通过时钟节拍的分布，判断时钟是否稳定

稳定时钟的时钟节拍分布均匀且不可调整，此时 is_steady() 静态数据成员为 true。在通常情况下，std::chrono::system_clock 是不稳定的；因为时钟可调，这种调节可能导致 now() 返回时间要早于上次 now() 的返回值。

C++ 标准库提供了稳定时钟 std::chrono::steady_clock。

**时延**

std::chrono::duration<> 函数模板能够对时延进行处理。

```cpp
// 时延的显式转换
// 隐式转换不支持对时延值进行截断
std::chrono::milliseconds ms(54802);
// 54 秒
std::chrono::seconds s = std::chrono::duration_cast(std::chrono::seconds)(ms);

// 时延支持算术运算
std::future<int> f = std::async(some_task);
if (f.wait_for(std::chrono::millisenconds(35)) == std::future_status::ready) {
    do_something_with(f.get());
}
```

**时间点**

时钟的时间点使用 std::chrono::time_point 的类型模板实例表示。

```cpp
auto start = std::chrono::high_resulotion_clock::now();
do_something();

auto stop = std::chrono::high_resulotion_clock::now();
std::cout << "do_something() took"
          << std::chrono::duration<double, std::chrono::seconds> (stop - start).count()
          << " second(s)."
          << std::endl;
```

```cpp
// 有超时功能的条件变量

#include <condition_variable>
#include <mutex>
#include <chrono>

std::condition_variable cond;
std::mutex m;
bool done;

bool wait_loop() {
    const auto timeout = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
    std::unique_lock<std::mutex> lk(m);

    while (!done) {
        if (cond.wait_until(lk, timeout) == std::cv_status::timeout) {
            break;
        }

        return done;
    }
}

```
## 使用同步操作简化代码

```cpp
FP-模式的递归排序
template<typename T>
std::list<T> sequential_quick_sort(std::list<T> input) {
    if (input.empty()) {
        return input;
    }

    std::list<T> result;
    result.splice(result.begin(), input, input.begin());

    const T& pivot = *result.begin();

    auto divide_point = std::partition(input.begin(), input.end(), [&] (const T& t) { return t < pivot; });

    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(), divide_point);

    auto new_lower(sequential_quick_sort(std::move(lower_part)));
    auto new_higher(sequential_quick_sort(std::move(input)));

    result.splice(result.end(), new_higher);
    reesult.splice(result.begin(), new_lower);

    return result;
}

// 并行版
template<typename T>
std::list<T> parallel_quick_sort(std::list<T> input) {
    if (input.empty()) {
        return input;
    }

    std::list<T> result;
    result.splice(result.begin(), input, input.begin());

    const T& pivot = *result.begin();

    auto divide_point = std::partition(input.begin(), input.end(), [&](const T& t) { return t < pivot; });

    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(), divide_point);
    
    std::future<std::list<T>> new_lower(std::async(&parallel_quick_sort<T>, std::move(lower_part)));

    auto new_higher(parallel_quick_sort(std::move(input)));

    result.splice(result.end(), new_higher);
    result.splice(result.begin(), new_lower.get());

    return result;
}
```

# 5 C++ 类型模型和原子类型操作

## 内存模型基础

在 C++ 程序中，所有的数据都是由对象构成的；在此，对象仅仅是对 C++ 数据构建块的一个声明，而C++ 标准定义类对象为存储区域。

**四个基本原则**

- 每一个变量都是一个对象，包括作为其成员变量的对象。

- 每个对象至少占有一个内存位置。

- 基本类型都有确定的内存位置。

- 相邻位域是相同内存中的一部分。

无论对象的类型是什么，它都会存储在一个或多个内存位置上。当两个线程访问不同的内存位置时，不会存在任何问题；当两个线程访问同一个内存位置时，如果没有线程更新内存位置上的数据，那也不会存在任何问题；*但当有线程需要对内存位置上的数据修改，那就有可能产生条件竞争。*

为了避免上述可能存在的条件竞争，两个线程需要按照一定的顺序执行。使用互斥量能够保证在同一时间内只有一个线程能够访问被待修改的内存位置，从而产生访问上的顺序。

在 C++ 程序中的对象，都有确定的修改顺序，该顺序在初始化阶段确定。一般地，该顺序不同于执行中的顺序，但在给定的执行程序中，所有线程都需要遵守这个顺序。

## 原子操作和原子类型

原子操作不可再分，这意味着原子操作是最细粒度的操作。在多线程中，原子操作只有只有两种情况：完成和未完成，它不存在任何其他中间状态。

标准原子类型的所有操作都是原子操作。

## 同步操作和强制排序

```cpp
#include <vector>
#include <atomic>
#include <iostream>

std::vector<int> data;
std::atomic<bool> data_ready(false);

void reader_thread() {
    while (!data_ready.load()) {
        std::this_thread::sleep(std::milliseconds(1));
    }

    std::cout << "The answer is " << data[0] << std::endl;
}

void writer_thread() {
    data.push_back(42);
    data_ready = true;
}
```

# 6 基于锁的并发数据结构设计

## 并发数据结构设计的意义

## 设计指导

## 实现为并发设计的数据结构

# 7 无锁并发数据结构设计

## 定义和意义

## 设计案例

## 设计指导

# 8 并发代码设计

## 线程间工作划分

## 影响并发代码性能因素

## 多线程性能该数据结构设计

## 注意事项

# 9 高级线程管理

## 线程池

## 处理任务依赖关系

## 获取任务

## 中断线程