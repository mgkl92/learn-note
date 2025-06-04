# 杂项之 Lambda 表达式

[原文连接](https://learn.microsoft.com/zh-cn/cpp/cpp/lambda-expressions-in-cpp?view=msvc-170)


## Lambda 的构成

![Lambda 的构成](/pics/consititution_of_lambda_expr.png)

1. Capture 字句：用户捕获在定义 Lambda 表达时需要的变量（所在作用域可见的变量）。

    - `[]`：不捕获任何变量；

    - `[&]`：通过引用捕获所有变量；
    
    - `[=]`：通过值捕获所有变量。

    ```cpp
    // 按引用捕获 total, 按值捕获 factor
    [&total. factor]

    // 作用同上
    [factor, &total]

    // 仅按值捕获 facotr, 其他则按引用捕获
    [&, factor]

    // 仅按引用捕获 total，其他则按值捕获
    [=, &total]

    // 使用 &, = 指示默认捕获模式时需作为首个元素
    // 使用默认捕获模式时，不能再使用相应的标识符来标记变量
    [&, &total] // Error
    ```

2. 参数列表（可选）：在调用 Lambda 表达式需要提供的输入参数。

    ```cpp
    auto y = [](int first, int second) {
        return first + second;
    };
    ```

3. mutable 规范

4. exception-specification 异常说明（规范）：用于指示 lambda 表达式可能产生的异常；

    当然，你也可以使用 `nonexcept` 来指示 Lambda 表达式不会引发任何异常。

    ```cpp
    auto = []() noexcept {
        throw 5; // Warning !
    };
    ```
5. trailing-return-type 尾置返回类型

    你可以使用 auto 加 TRT 来实现 Lambda 表达式返回类型的推断。默认，如果 Lambda 表达式仅包含单个返回语句，编译器将从返回表达式推导返回类型；否则，将会推导为 void。

    ```cpp
    // Return Type: int
    auto x1 = [](int i) { return i; };
    
    // Return Type: void
    // 无法从值列表推导类型
    auto x2 = []{ return {1, 2}; };
    ```

6. Lambda 函数体

    Lambda 表达式之恶能捕获具有自动存储持续时间的变量， 但可以在 Lambda 表达式中使用具有静态存储时间的变量。

    ```cpp
    void fillVector(vector<int>& ivec) {
        static int nextValue = 1;

        // 访问静态存储时间的变量
        generate(v.begin(), v.end(), [] { return nextValue++; });
    }
    ```

## constexpr Lmabda 表达式

当由 Lamdab 表达式引入的或者不获得数据成员为 constexpr 时，我们可以将 Lambda 修饰为 constexpr 并将其用在任何可以使用 constexpr 的地方。

```cpp
int y = 32;
auto answer = [y]() constexpr {
    int x = 10;
    return y + x;
};

constexpr int Increment(int n) {
    // 隐式的 constexpr Lambda 表达式
    return [n] { return n + 1 }();
}
```