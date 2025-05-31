## 1. 值传递 vs. 引用传递

- 值传递和引用传递是两种向函数传递参数的方式。在值传递方式中，形参作为实参的副本，对于形参的修改并不会直接影响到实参但存在拷贝操作；而在引用传递过程中，形参作为实参的引用，其本质上为实参的别名，对于形参的修改将直接作用于实参。

- 值传递在处理较大对象参数时其效率较低。

```cpp
// 值传递
void passByValue(int value) {
    value = 100;
}

// 引用传递
void passByRefer(int &value) {
    value = 100;
}

int a = 20;

// a = 20 
passByValue(a);

// a = 100
passByRefer(a);
```

## 2. 左值 vs. 右值

- 值根据能否出现在复制运算符的左侧分为左值和右值。左值可取地址，通常为变量；右值不可取地址，通常为常量、临时变量等。

```cpp
// a, b, c 均为左值
int a = 0, b = 1, c = 2;

// b + c 为右值
// &(b + c) 无效
a = b + c;
```

## 3. 移动语义 vs. 完美转发

- 移动语义并不对资源进行拷贝，而是对其所有权进行转移，其性能比拷贝更好。

```cpp
class A {
public:
    A(){}

    A(int size) : size_(size) {
        data_ = new int[size];
    }

    // copy 构造
    A(const A& a) {
        size_ = a.size_;
        data_ = new int[size_];
    }

    // move 构造
    A(A && a) {
        this->data_ = a.data_;
        a.data_ = nullptr;
    }

    ~A() {
        if (data_ != nullptr) {
            delete[] data_;
        }
    }
private:
    int size_;
    int * data_;
};

A a(10);

// copy
A b = a;

// move
A c = std::move(a);
```

- 完美转发

## 4. 列表初始化

- 列表初始化支持对基本数据类型、用户自定义数据类型（类、结构体等）以及容器类型的初始化操作。

- 列表初始化可以避免类型窄化。

```cpp
// 基本数据类型
int a = {10};

int arr[3] = {1 , 2, 3};

// 无效：类型窄化
int a = {3.14};

// 用户自定义类型
class Point{
public:
    int x, y;
    Point(int a, int b) : x{a}, y{b} {}
};

Point point{0, 0};

struct Aggregate {
    int a;
    double b;
};

Aggregate agg = {1, 2.3};

// 容器类型
vector<int> vec = {1, 2, 3};
```

## 5. 智能指针

- C++ 采取智能指针进行资源管理以避免内存泄露。

### 5.1 unique_ptr 

```cpp
class Test{
public:
    Test() { cout << "Test::Test()" << endl; }
    ~Test() { cout << "Test::~Test()" << endl; }
    void test() { cout << "Test::test()" << endl; }
}

unique_ptr<Test> uptr(new Test());
// Output: Test::Test()

uptr->test();
// Output: Test::test()

// 当 uptr 离开时作用域时，Test 对象的析构函数会被调用
// Output: Test::~Test()
```

### 5.2 shared_ptr

```cpp
shared_ptr<Test> sptr1(new Test());
// Output: Test::Test()

shared_ptr<Test> sptr2 = sptr1;
// No Output

sptr2->test();
// Output: Test::test()

// 当 sptr1 和 sptr2 离开时作用域时，Test 对象的析构函数会被调用
// Output: Test::~Test()
```

### 5.3 weak_ptr

```cpp
shared_ptr<Test> sptr(new Test());
// Output: Test::Test()
weak_ptr<Test> wptr = sptr;
// No Output

if (auto lockedSharedPtr = wptr.lock()) {
    lockedSharedPtr->test();
    // Output: Test::test()
}

// 当 sptr 离开时作用域时，Test 对象的析构函数会被调用
// Output: Test::~Test()


// 循环引用
class B;
class A{
public:
    shared_ptr<B> sptr;
    ~A(){ cout << "A has been destroyed" << endl; }
};

class B{
public:
    shared_ptr<A> sptr;
    ~B(){ cout << "B has been destroyed" << endl; }
};

shared_ptr<A> a = make_shared<A>();
shared_ptr<B> b = make_shared<B>();
a->sptr = b;
b->sptr = a;
// No Output !!!

// 即使当 a 和 b 离开作用域时，A 和 B 对象的析构函数因为循环引用将不会被调用。
```
 
### 智能指针的不足

## 6. static 关键字

### 6.1 修饰局部变量

```cpp
void func() {
    // count 仅在第一次函数调用时初始化
    static int count = 0;
    ++count;
}

for (size_t i = 0; i < 5; ++i) {
    func();
}
```

### 6.2 修饰全局变量或函数
默认情况下，文件中的全局变量和函数具有外连接性，即能够被其他文件导入使用，但 static 关键字能够禁用这种特性。
```cpp
// file1.cpp
static int count = 10;
static void func() {
    cout << "func() in file1." << endl;
}

// file2.cpp
// 无效：编译错误
extern int count;
```

### 6.3 修饰成员变量 / 成员函数

```cpp
class MyClass{
public:
    static int val;
    static void func() { ... }
}

// 静态成员初始化
int MyClass::val = 10;

// 调用静态成员函数
MyClass::func();
```

## 7. const 关键字

const 关键字修饰的变量除初始化外不可变更其值。

```cpp
// 1. 定义常量
const int MAX_SIZE = 100;

// 2. 常引用
void func(const int & a) { ... }

class MyClass {
public:
    // 3. 修饰类成员函数
    void func() const { ... }

    // 4. 修饰类成员变量
    const int val = 5;
}

```

### 7.1 #define vs. const

- 预处理指令在预处理阶段被展开，即宏展开，其基本机制为文本替换。所以，通过预处理指令定义的常量并不会出现在编译阶段，编译器也不会为之分配存储空间，故不具有类型安全。此外，使用预处理指令可能面临代码膨胀问题。
  
- 预处理指令不具备作用域，仅在源文件范围内有效，可使用 #undef 取消宏定义。

```cpp
// #define 宏定义
#define PI 3.14

double area = PI * 10 * 10;

// const 常量
const double PI = 3.14;
double area = PI * 10 * 10; // 类型安全
```

### 7.2 const 与指针

```cpp
// 1. 指向常量的指针（并非指针为常量）
const int *ptr_ci = &x;

ptr_ci = &y; // 有效：修改指针对象的内容
*ptr_ci = 2; // 无效：修改指针所指对象的内容

// 2. 指针常量
int * const cptr_i = &x;

cptr_i = &y; // 无效：修改指针对象的内容
*cptr_i = 2; // 有效：修改指针所指对象的内容

// 3. 指向常量的常量指针
const int * const cptr_ci = &x;

cptr_ci = &y; // 无效：修改指针对象的内容
*cptr_ci = 2; // 无效：修改指针所指对象的内容
```

## 8. inline 关键字

- 函数的内联化指的是编译器将对函数的调用替换为其函数体，能够减少函数调用的开销。内联函数一般体积较小且在程序执行期间被频繁调用。

- `inline` 关键字一般只是作为编译器的优化建议，这并不代表函数一定会在编译期间展开。

- 函数内联展开的行为类似预处理指令的定义，但函数的内联化为编译阶段行为且具有类型安全性。

## 9. 数组与指针

- 编译器在栈上为数组的所有元素分配连续的存储空间，数组的大小在便阶段确定，在运行阶段保持不变。而指针本质上仍为变量，其内容为内存地址，可通过动态内存分配存储空间。

- 数组和指针均具有类型安全性。数组元素类型在编译阶段完成绑定，而指针指向内容的类型可重新分配（前提是具有兼容性）。

- 数组名在本质上为指向数组首元素的指针常量，不能够对其进行 `++` 和 `--` 操作，但允许指针间的算术操作（指针加上一个偏移，两个指针作差等）。

- 对数组名使用 sizeof 返回的是数组元素占用的字节数目，而对指针使用 sizeof 返回的是指针变量占用字节数（由地址的位数决定）。
- 
```cpp
// 在表达式中，数组名会被自动转化为指向数组首元素的地址。
int arr[5];

// arr -> &arr[0]
```

## 10. explicit 关键字

`explicit` 关键字用于防止构造函数或转换函数在不合适的情况下被隐式调用。

```cpp
// 避免单参构造函数隐式转换
class Bar {
public:
    explicit Bar(int x) : value(x) {}
private:
    int value;
};

// 无效：隐式转换
Bar bar = 10;

// 避免隐式转换操作隐式
class Double{
public:
    explicit operator int() const {
        return static_cast<int>(value);
    }
private:
    double value;
};

Double d;

// 无效：隐式转换
int i = d;

// 有效
int j = static_cast<int>(d);

// 避免复杂构造函数中的隐式转换
class Widget {
public:
    explicit Widget(int x = 0, bool flag = true): value(x), flag(flag) {}
private:
    int value;
    bool flag;
};
```

## 11. final 关键字

`final` 关键字用于防止类被继承或虚函数被覆盖。

```cpp
// 防止类被继承
class Base final {
    ...
};

// 无效：Base 类无法被继承
class Derived : public Base {
    ...
};

// 防止虚函数被覆盖
class Base {
public:
    virtual void func() final {
        ...
    }
};

class Derived : public Base {
public:
    // 无效： func() 无法被覆盖
    // override 关键字用于指出该方法为覆盖基类中某个虚函数的定义
    virtual void func() override {
        ...
    }
};
```

## 12. 内存对齐



## 13. 类型转换

**`static_cast`** 用于有明确类型之间的转换，如基本数据类型。

```cpp
int a = 10;
float b = static_cast<float>(a); // 仅在编译阶段进行类型检查
```

**`dynamic_cast`** 主要用于多态类型的指针或引用之间的转换。

```cpp
Base *base_ptr = new Derived();
Derived *derived_ptr = dynamic_cast<Derived *>(base_ptr); // 仅当类包含虚函数时使用
```

**`const_cast`** 用于添加或移除常量修饰。

```cpp
const int a = 10;

int *b = const_cast<int *> (&a); // 仅影响底层 const 属性？
*b = 20; // 修改原本为 const 的值
```

**`reinterpret_cast`**

```cpp
long p = 0x12345678;
int *i = reinterpre_cast<int *>(p); // 无类型安全性检查
```
## 14. volatile 关键字

## 15. 多态

- 静态多态：在程序编译时完成绑定

```cpp
int add(int a, int b) {
    return a + b;
}

char add(int a, int b) {
    return a + b;
}

add(1, 2);
add('1', '2');
```

- 动态多态：在程序运行时完成绑定

    **条件**
    
    - 虚函数
    
    - 通过基类引用/指针调用虚函数

```cpp
class Base{
public:
    virtual Base* test() {
        std::cout << "victory" << std::endl;
        return this;
    }
};

class Derived : public Base {
public:
    virtual Derived* test() {
        std::cout << "yeah" << std::endl;
        return this;
    }
};

Derived d;
Base  *b = &d;

// Derived::test()
b->test();
```

## 16. 虚函数

虚函数是 C++ 实现多态的关键机制。

虚函数允许你在基类中使用 `virtual` 关键字修饰函数，然后在其派生类里对该虚函数重新定义。C++ 可以根据对象的实际类型（而非引用或指针的静态类型）调用派生类的函数实现。

虚函数实现依赖于虚函数表（vtable）和虚函数表指针（vptr）。每个含有虚函数的类都有一张虚函数表，该表中存储类中虚函数的地址。每个对象都有一个虚函数表指针，该指针指向这个类的虚函数表。在函数调用时，程序会通过对象的虚函数表指针找到相应虚函数的地址，然后进行函数调用。

```cpp
class Base{
public:
    virtual void func() { ... }
};

class Derived : public Base {
public:
    void func() override { ... }
};

Base *base_ptr;
Derived derived;
base_ptr = &derived;

base_ptr->func(); // 调用 Derived 类的 func() 函数

// 包含纯虚函数的类不能够被实例化，只能被继承！
class AbstractBase {
public:
    virtual void func() = 0;
};
```

注：因为，在对象初始化阶段虚函数表对象仍未被建立并初始化；所以，构造函数不能够被 `virtual` 关键字修饰。

注：析构函数可以被 `virtual` 关键字修饰，这样可以保证派生类对象能够被正确释放，从而避免资源泄露。

注：由于静态成员函数与类而非某个具体对象关联，而友元函数其本质上并非类的成员函数；因此，静态成员函数和友元函数也不能够被 `virtual` 关键字修饰。

**纯虚函数** 在 vtable 中被赋值为 0，表示指向一个不存在的函数。

**菱形继承问题**

```cpp
class A { ... };

// 非虚继承
class B : public A { ... };
class C : public A { ... };
// D 类对象中包含继承自 B 的 A 类对象和继承自 C 的 A 类对象且这两个子类对象相互独立
class D : public B, public C { ... }; 

// 虚继承
class B : virtual public A { ... };
class C : virtual public A { ... };
// 虚继承使得 D 类对象中仅包含一个 A 类对象
class D : public B, public C { ... }; 
```

虚继承确保在派生类中，仅包含一个基类实例，从而能够避免访问基类成员时的歧义问题。但虚继承会影响类的内存布局，因为编译器需要增加额外的指针或控制块来管理菱形结构中基类的单一实例。

## 17. struct vs. class

!C++ 对 C 中的 `struct` 进行了扩充，使其能像 `class` 一样进行工作。

在 C++ 中， 使用 `struct` 和 `class` 关键字均可以定义类，区别在于类中成员的默认访问级别：前者为 `public`，而后者为 `private`。此外，在继承关系中也遵循上述规则。

在类模板的参数定义中，可以使用 `class` 替换 `typename`, 但不能使用 `struct` 进行替换。

## 18. struct vs. union

在 struct 中，所有的成员变量占据独立的内存空间；而 union 的所有成员变量共享一块内存空间且该空间大小由最大数据成员所需存储空间决定。

```cpp
struct Point {
    int x;
    int y;
};

// struct 所有成员变量可以同时存在并被访问
Point p;
p.x = 3;
p.y = 4;

union Single {
    int a;
    int b;
};

// union 有且仅有一个成员变量存在且被访问
// 如果在一个成员值发生改变后访问另外一个成员，结果不可预知！
Single s;
s.a = 10;
s.b = 11;
```

## 19. using vs. typedef

`using` 和 `typedef` 均可以为已有类型定义别名，区别在于：`using` 可以为模板定义别名，而 `typedef` 不可以。

```cpp
// 使用 typedef 定义类型别名
typedef unsigned long ulong;
typedef int (*func_ptr) (double);

// 使用 using 定义类型别名
using ulong = unsigned long;
using func_ptr = int (*) (double);

// 使用 using 定义模板别名
template <T>
using Vector = std::vector<T>;
Vector<int> vec; // 等价于 std::vector<int> vec;

// 使用 using 命名空间引入
namespace LongNamespace {
    int value;
}

using LN = LongNamespace;
LN::value = 42; // 等价于 LongNamespace::value = 42;
```

## 20. enum vs. enum class

在 C++ 中，`enum` 和 `enum class` 主要区别在于作用域和类型安全。

**作用域**

- `enum` 的枚举成员直接进入其包含作用域，即可以直接使用其枚举成员，无需前缀。

- `enum class` 的枚举成员只能通过显式的指定其枚举类型访问，即使用枚举名作为前缀。

**类型安全**

- `enum` 的枚举成员会被隐式转化为整数。

- `enum class` 是强类型枚举，不能隐式的转化为其他类型，必须显式转换。


```cpp
enum Color {
    RED,
    GREEN,
    BLUE
};

Color color = RED; // 无前缀直接访问
int color_code = RED; // 隐式转化

enum class Color {
    RED,
    GREEN,
    BLUE
};
 
Color color = Color::RED; // 前缀访问呢
int color_code = Color::RED; // 无效：不支持隐式转换
``` 

## 21. new && delete vs. malloc && free

- new 和 delete 是操作符，而 malloc 和 free 为函数；

- 使用 new 创建对象时，无需指定对象的大小，new 先为对象分配内存空间，然后调用对象的构造函数；

- malloc 需要指定申请内存空间的大小，返回 void 类型指针，需要使用强制类型转换将 void 指针转化为目标对象的指针；

- new 关键字具有类型安全性；

- new 分配失败时将抛出异常，而 malloc 则返回 NULL。

## 22. delete 关键字和 default 关键字

- `delete` 关键字用来禁用某些默认的成员函数。

- `default` 关键字用于显式地指示编译为某个成员函数生成默认实现。

```cpp
class NonCopyable {
public:
    NonCopyable() = default;
    NonCopyable(const NonCopyable &) = delete;
    NonCopyable &operator=(const NonCopyable&) = delete;
}；
```
## 23. this 指针

`this` 指针是隐含在每一个非静态成员函数中的指针，它用来指向调用该成员函数的对象。

```cpp
// 1. 访问类非静态成员变量和成员函数
class Box {
public:
    Box(double length) {
        this->length = length;
    }

private:
    double lenght;
};

// 2. 链式调用
class Box {
public:
    Box & set_length(double length) {
        this->length = length;
        return *this;
    }

    void display() {
        cout << "The length is " << endl;
    }
}

Box box;
box.set_length(5.0).display();

// 3. 动态绑定
class Base {
public:
    virtual void show() { ... }
    
    void display() {
        this->show();
    }
};

class Derived : pubic Base {
public:
    void show() override { ... }
};
Derived derived;
Base *base_ptr = &derived;
// Derived 对象的 show 成员函数将被调用
base_ptr->display();
```

## 24. auto & decltype

`auto` 和 `decltype` 关键字自 C++ 11 引入，其作用为类型推断。

`auto` 用于自动推断变量的类型。编译器将根据变量的初始化表示来推导变量的类型。

```cpp
auto x = 10; // x -> int
auto y = 3.14; // y -> double
auto s = "Hello, World!"; // s -> string
```

`decltype` 用于推断表达式的类型，它将返回一个表达式对应的类型信息，该过程并不会正在的计算相应表达式！

```cpp
int x = 10;
decltype(a) y = 20; // y -> int
decltype(a + 10.0) z = 30; // z -> double
```

```cpp
// example 1
vector<pair<int, string>> vec;

for (vector<pair<int, string>>::iterator it = v.begin(); it != v.end(); ++it){ ... }

// 使用 auto 进行类型的自动推导 
for (auto it = v.begin(); it != v.end(); ++it) { ... }

// example 2
template<typename T, typename U>
auto add(T a, U b) -> decltype(a + b) {
    return a + b;
}
```

## 24. 堆内存 vs. 栈内存

- 分配方式：栈内存由编译器在程序运行时自动分配和释放；而堆内存需要程序手动分配和释放。

- 管理方式：栈内存由编译自动管理，生命周期由其作用域决定；而对内存由程序员手动进行管理，如果内存没有被正确释放将导致内存泄露。

- 生命周期：变量在离开作用域后自动销毁；而堆内存需要手动释放，否则将持续存在至程序结束。

- 性能：堆内存分配和释放速度快；而堆内存涉及到复杂内存管理和分配机制，性能较慢。

栈空间有限，如果递归过深或者分配局部变量太大，将导致栈空间溢出（所需空间大于栈内存空间）。

堆内存如果没有被正确释放，将导致内存泄露，从而影响系统的整体性能。

## 25. nullptr vs. NULL

`nullptr` 有明确的类型，其类型为 `std::nullptr_t`，可以避免代码中出现类型不一致的问题，具有类型安全性。

`NULL` 通常被定义为数组 `0`，其实际上为整型值，这可能引发类型不一致问题，如在参数传递时，编译器无法判断是整数 `0` 还是空指针！

```cpp
// 代码可读性差

// example 1
void func(int x) { ... }

void func(void * ptr) { ... }

// 调用？
func(NULL);

// 调用 func(void *ptr)
func(nullptr);

// example 2
template<typename T>
void foo(T x) { ... }

// int
foo(0);
// ？
foo(NULL);
// std::nullptr_t
foo(nullptr);
```

## 26. #include "a.h" 和 #include <a.h>

使用 <>， 编译器会在预定义的系统安装目录中搜索头文件，主要用于标准库和第三方库的头文件。
使用 ""， 编译器会先在当前源文件所在目录和自定义目录中搜索头文件；然后，再在系统预定义的安装目录中进行搜索。

```cpp
// 预定义的系统安装目录
#include <iostream>
#include <string>
#include <vector>

// 当前目录 / 预定义的系统安装目录
#include "a.h"
```

## 27. 浅拷贝 vs. 浅拷贝

浅拷贝与深拷贝的区别在于深拷贝会拷贝对象所持有的内存或资源，深拷贝对象与被拷贝对象之间对内存或资源的操作相互独立。

```cpp
#include <cstring>

using std::strlen;
using std::strcpy;

class MyClass {
private:
    char *data;
public:
    MyClass(const char * input) {
        data = new char[strlen(input)];
        strcpy(data, input);
    }

    // 深拷贝
    MyClass(const MyClass & other) {
        data = new char [strlen(other.data)]
        strcpy(data, other.data);
    }

    // 赋值拷贝
    MyClass & operator(const MyClass & other) {
        if (this != &other) { // self-assignment
            delete[] data; // 先释放资源，再拷贝资源
            data = new [strlen(other.data)];
            strcpy(data, other.data);
        }

        return *this;
    }

    ~MyClass() {
        delete[] data;
    }
};

MyClass obj("Hello, World!");
MyClass copied_obj = obj;
```
## 28. 命名空间 `namespace`

命名空间主要用于解决名称冲突的问题。

```cpp
namespace mgkl {
    int x;
    void func() { ... }

    ...
}
// 访问
mgkl::x;
mgkl::func();

// 嵌套命名空间
namespace Outer {
    namespace Inner {
        int x;
        void func() { ... }
    }
}
// 访问
Outer::Inter::x;
Outer::Inter::func();

// 匿名命名空间
// 不能够在文件外访问，即禁用外连接特性
namespace{
    int x;
    void func() { ... };
}
```
## 29. 引用和指针

指针是一个变量，存储的是地址。

引用与原来变量实质上是同一个变量，引用为原先变量的别名，引用本身并不存储指针，而是在底层通过指针来实现对原先变量的访问。

```cpp
int a = 10, b = 20;

int &ra = a; // 有效
int &rb; // 无效：引用必须被初始化
ra = b; // 无效：引用一旦被创建不能够重新赋值

int *pa = &a; // 有效
int *pb; // 有效
pb = &b; // 有效
pb = nullptr; // 有效
```

# 进阶

## 30. 程序的生命周期

1. 编写代码

2. 预处理
  
- 头文件 `#include`

- 宏定义 `#define`

- 条件编译 `#ifdef`, `#ifndef` ...

3. 编译

- 词法分析
  
- 语法分析

- 语义分析

4. 汇编

5. 链接

- 符号解析

- 地址分配

- 重定义

## 31. new & delte, new [] & delete[] 成对使用

当使用 `new[]` 分配连续的内存时，编译器不仅管理数据的实际存储位置，还存储了数组的大小信息，使得 `delete[]` 能够正确释放内存。

- 使用 `new[]` 分配内存空间但是用 `delete` 释放，数组中的每个对象的析构函数不会被调用（除数组地址指向的首个数据元素），导致内存泄露。
  
- 使用 `new` 分配内存空间但是使用 `delete[]` 释放，可能导致未定义行为（这取决于存储数组大小信息存储空间的内容以及程序的运行状态）！

注：在面对基本数据类型时，可以不成对使用，即使这样也不会造成内存泄露（基本数据类型不含析构函数）。

```cpp
#include <stdlib.h>
#include <iostream>

using namespace std;

class inner {
public:
    inner() { cout << "inner()" << endl; }
    ~inner() { cout << "~inner()" << endl; }
};

inner *p = new inner[2];
delete p;

/** Output:
* inner()
* inner()
* ~inner()
*/

inner *p = new inner();
delete[] p;

/** Output:
* inner()
* ~inner()
* ~inner()
* ...
* Aborted (core dumped)
*/
```

## 32. unique_ptr 实现原理

智能指针 `unique_ptr` 由 C++ 11 引入，遵循 RAII 规则，自动管理资源的动态分配与释放。

由 `unique_ptr` 管理的资源具有独占特性，一旦被分配，不允许复制，仅能使用 `std::move` 进行所有权的转移。这种独占性源自在 C++ 实现中禁用了拷贝构造和赋值运算符。

```cpp
unique_ptr(const unique_ptr &) = delete;

unique_ptr & operator=(const unique_ptr&) = delete;

template<typename _Up, typename _Up_Deleter>
unique_ptr(const unique_ptr<_Up, _Up_Deleter> &) = delete;

template<typename _Up, typename _Up_Deleter>
unique_ptr & operator=(const unique_ptr<_Up, _Up_Deleter> &) = delete;
```

```cpp
// 创建 unique_ptr
unique_ptr<int> ptr1(new int(10));
auto ptr2 = make_unique<int>(20);

// 转移所有权
unique_ptr<int> ptr3 = move(ptr1);
```

## 33. shared_ptr 实现原理

- 智能指针 `shared_ptr` 使用引用计数来记录指向当前资源引用数目。

- 当构建新的对象时，引用计数初始化为 1；当拷贝对象时，引用技术加 1；当对象作用域结束析构时，引用计数减 1；当引用计数减少为 0 时，资源被释放。

- share_ptr 具有线程安全性，即保证多个线程能够安全的增加或减少引用计数，但并不保证其所持资源的线程安全性。

```cpp
// 手动实现
class SharedCount {
public:
    SharedCount() : count_{1} {}
    void add() { ++count_; }
    void minus() { --count_; }
    int get() const { return count_; }
private:
    std::atomic<int> count_;
};

template<typename T>
class SharedPtr {
public:
    SharedPtr() : ptr_{nullptr}, ref_count_{new SharedCount} {}
    SharedPtr(T* ptr) : ptr_{ptr}, ref_count_{new SharedCount} {}

    SharedPtr(const SharedPtr& other) {
        this->ptr_ = other.ptr_;
        this->ref_count_ = other.ref_count_;
        // 增加引用计数
        ref_count_->add();
    }

    SharedPtr & operator=(const SharedPtr & other) {
        if (&other != this) {
            this->ptr_ = other.ptr_;
            this->ref_count_ = other.ref_count_;
            ref_count_->add();
        }
        return *this;
    }

    // 移动语义
    SharedPtr(SharedPtr && other) {
        this->ptr_ = other.ptr_;
        this->ref_count_ = other.ref_count_;
        other.ptr_ = nullptr; 
        other.ref_count_ = nullptr;
    }

    SharedPtr & operator=(SharedPtr && other) {
        if (&other != this) {
            clean();

            this->ptr_ = other.ptr_;
            this->ref_count_ = other.ref_count_;
            other.ptr_ = nullptr; 
            other.ref_count_ = nullptr;
        }

        return * this;
    }

    ~SharedPtr() { clean(); }
private:
    T* ptr_;
    SharedCount * ref_count_;

    void clean() {
        if (ref_count_) {
            // 减少引用计数
            ref_count_->minus();
            // 释放资源
            if (ref_count_-get() == 0) {
                if (ptr_) {
                    delete ptr_;
                }

                delete ref_count_;
            }
        }
    }
};
```

## 34. weak_ptr 实现原理

智能指针 `weak_ptr` 主要用来解决 `shared_ptr` 中存在的循环引用问题。

`weak_ptr` 基于 `shared_ptr` 实现，本身并不管理资源，而是与 `shared_ptr` 共享内部的控制块，该控制块包含指向实际资源的指针和引用计数。
当所有的 `shared_ptr` 对象被销毁时，资源被释放；但控制块将直到所有的 `weak_ptr` 对象销毁才释放。 

```cpp
// 成员函数

// 将 weak_ptr 转换为 shared_ptr
lock() 

// 检查 weak_ptr 指向资源是否已经被释放
expired() 
```

## 35. make_shared 

```cpp
class A {
    ...
};

// 方式 1 
auto s_ptr = shared_ptr<A>(new A());

// 方式 2 (推荐)
auto s_ptr = make_shared<A>()；
```

方式 1 可能存在两次动态内存的分配，即资源所需内存以及 `shared_ptr` 智能指针所需内存（控制块）的分配；而方式 2 可一次性完成上述两个工作。

## 36. shared_from_this

## 37. 静态库 vs. 动态库

- 动态库在运行时被加载，提供共享地库文件（如 .dll, .so）；而静态库（如 .lib, .a）在编译时将被直接合并到可执行文件中。

- 动态库在内存中被多个程序共享，可以节省内存空间和磁盘空间，但会引入加载开销；而对于静态库，每个使用它的程序都会有一份拷贝，二进制文件较大，但启动速度较快。

- 动态库在升级时，无需重新编译依赖于该库的程序，只需更新库文件即可；而静态库的升级，需要重新编译依赖于该库的程序。

- 静态链接在编译阶段就进行符号解析，即所有函数和变量的引用都将在编译阶段解决；而动态链接在运行时进行符号解析，会增加函数和变量的加载时间。

- 在创建动态库时，需要使用 `-fPIC` 选项来生成在任意内存地址运行的代码。

- 静态库的连接顺序可能会影响最终的可执行文件，而动态库的依赖关系可以在运行时解决。

## 38. 单例模式

**饿汉模式** 实例在程序开始运行时就被创建，如直接在类中初始化静态成员。

**懒汉模式** 实例在首次使用时才被创建，如静态局部变量。

```cpp
class Singleton {
private:
    // 将构造函数设置为 private
    Singleton() {}
    // 禁用拷贝构造函数和赋值操作符
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
public:
    static Singleton& getInstance() {
        // 利用局部静态变量的线程安全性，确保多线程环境下仅创建一个实例（C++11）
        static Singleton instance;
        return instance;
    }
    ...
};
```

```cpp
class Singleton {
private:
    Singleton() {}
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    static std::mutex mutex_; // 使用互斥锁显式保证线程安全
public:
    static Singleton& getInstance() {
        std::lock_guard<std::mutex> lock(mutex_);
        static Singleton instance;
        return instance;
    }
    ...
};
std::mutex Singleton::mutex_;
```

```cpp
// 双重检测机制
class Singleton {
private:
    Singleton() {}
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    static Singleton * instance_;
    static std::mutex mutex_; // 使用互斥锁显式保证线程安全
public:
    static Singleton& getInstance() {
        if (instance_ == nullptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (instance_ == nullptr) {
                instance_ = new Singleton();
            }
        }

        return instance_;
    }
    ...
};

Singleton* Singleton::instance_ = nullptr;
std::mutex Singleton::mutex_;
```

## 39. string 内部使用堆内存还是栈内存

`std::string` 对象本身位于栈内，它所管理的字符串数据存储于堆内存中；但时，当字符串比较短时，部分实现可能会在 std::string 对象内部使用一个短字符串优化（SSO），此时字符串数据会位于栈内存中。

## 40. 锁（lock）与原子变量（atomic）

锁和原子变量都是 C++ 中的同步机制。

**锁的使用场景**

- 保护长时间访问的临界区，如复杂的操作逻辑（复杂数据结构的操作）。

- 多个共享资源需要同步访问，一次性锁定多个资源，确保整体一致性。

**锁的类型**

- 互斥锁（Mutex）：资源独占

- 读写锁（Read-Write Lock）：读并行，写独占

- 自旋锁（Spin Lock）：线程在等待是会不断轮询锁状态而非挂起

```cpp
std::mutex mutex_;
std::map<int, std::string> shared;

void insertIntoMap(int key, const std::string & value) {
    std::lock_guard<std::mutex> lock(mutex_);
    shared[key] = value;
}
```

**原子变量的使用场景**

- 操作可以在一个原子步骤内完成，如简单的整数增加或减少、标志位切换等。

- 性能优先，因为锁的开销和上下文切换成本过高。

- 用于实现非阻塞算法，原子变量不会导致线程挂起而等待锁释放。

```cpp
std::atomic<int> counter(0);

void increse() {
    ++counter;
}

void decrese() {
    --counter;
}
```

### 死锁

当两个或多个线程互相等待对方释放锁以继续执行时，发生死锁。

**死锁条件**

- 互斥条件

```cpp
std::mutex mutex_;

void func() {
    std::lock_guard<std::mutex> lock(mutex_);
    // 临界区代码
}
```
  
- 占有且等待

std::mutex mutex_a, mutex_b;

void thread_func() {
    std::unique_lock<std::mutex> lock_a(mutex_a, std::defer_lock);
    std::unique_lock<std::mutex> lock_b(mutex_b, std::defer_lock);
    // 脱离单独锁定合并为原子操作
    std::lock(lock_a, lock_b);
    // 临界区代码
}

- 不可剥夺

```cpp
std::mutex mutex_a, mutex_b;
void thread_func() {
    while (true) {
        mutex_a.lock();
        if (mutex_b.try_lock()) {
            // 临界区代码

            mutex_b.unlock();
            mutex_a.unlock();
            break;
        } else {
            mutex_a.unlock();
            std::this_thread::yield();
        }
    }
}
```

- 环路等待

```cpp
std::mutex mutex_a, mutex_b;
void thread_func1() {
    // 遵循资源请求顺序
    std::lock(mutex_a, mutex_b); 
    std::lock_guard<std::mutex> lock_a(mutex_a, std::adopt_lock);
    std::lock_guard<std::mutex> lock_b(mutex_b, std::adopt_lock);
}

void thread_func2() {
    // 遵循资源请求顺序
    std::lock(mutex_a, mutex_b); 
    std::lock_guard<std::mutex> lock_a(mutex_a, std::adopt_lock);
    std::lock_guard<std::mutex> lock_b(mutex_b, std::adopt_lock);
}
```

解决死锁问题只需破坏以上四个条件之一即可。

- 避免互斥条件：减少资源的独占性，使用非阻塞同步机制。

- 破坏占有且等待：采用资源预分配策略，一次性请求所需的所有资源。

- 破话不可剥夺：如果进程的不过需要资源，应该释放它所持有的资源或者使用优先级来剥夺资源

- 破话环路等待：对系统中的资源进行排序，每个线程按序请求资源，避免形成环路。

## 41. 继承（Is-a） vs. 组合(Has-a)

**继承使用场景**

- 两个类之间的关系是 Is-a 时，如 Dog 是 Animal。

- 派生类要对基类的行为进行拓展或修改并能够利用多态性质时。
  
- 基类提供了通用的功能，而派生类则基于这些功能进行特化。

**优点**：代码复用，多态性

**缺点**：强耦合

**里氏替换原则** 确保派生类能够代替基类使用。

**组合使用场景**

- 两个类之间的关系是 Has-a 时，如 Car 有多个 Engine。
  
- 复用其他类的功能，而非继承它们。

- 系统的不同部分以不同的方式进行组合，组合便于维护和拓展。

**优点**：灵活性高，低耦合

**缺点**：设计复杂性增加，性能开销增加

**指责单一原则** 每个组件应该尽量保持单一职责，避免单一组件承担过多功能。

## 42. 返回值优化（Return Value Optimization，RVO）

返回值优化是 C++ 编译器的一种优化技术，它主要针对返回值临时对象的创建和析构进行优化。编译器可以在某些情况下省略临时对象的构建和析构，直接在返回位置构建对象，从而提高程序的性能。

```cpp
class MyClass {
public:
    MyClass() { ... }
    MyClass(const MyClass&) { ... }
    ~MyClass() { ... }
};

// RVO
MyClass func() {
    return MyClass();
}

// NRVO
MyClass func() {
    MyClass obj;
    ...
    return obj;
}
```

## 43. 野指针

野指针指向的位置不可知，从而导致在进行内存访问时，产生非法访问的错误。

**避免**

- 将指针初始化如 nullptr

- 释放指针指向内容后，将其置 nullptr

- 使用智能指针进行资源管理

## 44. 内联函数 vs. 宏函数

- 宏函数和内联函数分别在程序的预处理阶段和编译阶段展开；

- 宏函数只是简单的字符替换，不具类型安全性；

- 内联函数在编译时将对类型的合法性进行检查。

## 45. C++ 传值方式

- 值传递

- 指针传递

- 引用传递

## 46. 内存泄露场景

- 使用 new 和 malloc 申请内存资源时，未使用 delete 和 free 进行释放；

- 在继承关系中，如果父类的析构函数不为虚函数时，通过父类指针或引用指向子类对象，将导致仅有父类部分被正确析构。

## 47. 重载 vs. 重写

- 重写指的是在派生类中重新定义基类中的函数，该函数除函数体外与基类函数保持一致；

- 重载允许参数列表具有不一致性！

```cpp
#include <bits/stdc++.h>
using namespace std;

class A {
    void func() {}

    void func(int i) {}
}; 
```

**如何在 C 语言中实现重载？**

- 使用函数指针来实现，尽管如此重载的函数仍不能使用相同的名称；

- 重载函数使用可变参数

```cpp

#include <stdio.h>

void func_int(void *a) { ... }

void func_double(void *b) { ... }

typedef void (*ptr)(void *);

void c_func(ptr p, void *param) {
    p(param);
}

```

## 48. C++ 类对象的初始化顺序

- 基类的构造函数

- 成员类的构造函数

- 派生类的构造函数

## 49. 虚析构 Vs. 虚构造

- 使用派生类类型指针指向派生类的实例，不管基类的析构函数是否为虚函数，都能够正常析构；

- 用基类类型指针指向派生类实例，如果基类的析构函数不是虚函数，则将只会析构基类实例，而不是析构派生类对象实例。

**不能使用虚构造函数**

- 从存储空间角度来讲，虚函数对应的 vtable 存储于对象的内存空间中，而在调用构造函数时，对象的存储空间还尚未分配！

## 50. 模板类的实现

- 模板实例化

    - 显式实例化

    - 隐式实例化

- 模板具体化：当使用模板实例化生成的类或函数不能够满足需求是，模板具体化允许修改原模板的定义。

```cpp
template<typename T>
struct TemplateStruct {
    TemplateStruct() { 
        std::cout << sizeof(T) << std::endl;
    }
};

// 模板显式实例化
template struct TemplateStruct<int>;

// 模板具体化
template<> struct TemplateStruct<double>{
    TemplateStruct() {
        std::cout << "-- 8 --" << std::endl;
    }
}

// 模板隐式实例化
TemplateStruct<char> temp;
```

## 50. 类的引用数据成员的初始化

- 不能使用默认构造函数进行初始化

- 构造函数的形参也必须时引用类型

- 不能再构造函数体内进行初始化，仅能在初始化列表中进行初始化

## 51. 不能虚化的函数

- 普通函数（非成员函数）：只能够被重载，不能够被覆盖，因为普通函数本身不属于任何类对象，故也不会存在于类的继承体系中；

- 友元函数：与普通函数相比，友元函数能够访问对象的内部状态，但 C++ 并不支持友元函数的继承（友元函数本身也不属于任何类对象）；

- 构造函数

- 静态成员函数：属于类而非某个类对象，不存在隐式 this 指针，故无法进行对象的判别。

- 内联成员函数：内联函数在编译期间展开，而虚函数在运行时进行绑定，二者在行为上矛盾。