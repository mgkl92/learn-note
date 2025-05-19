# 序列容器

容器用来存储某个指定类型的对象。

**序列容器**
- vector
- list
- deque

**序列容器适配器**
- stack
- queue
- priority_queue

## 定义序列容器

```c
#include<vector>
#include<list>
#include<queue>

vector<string> svec;
list<int> list;
deque<Sales_item> items;
```

通过默认构造器定义的容器对象为空，即不存储任何数据元素。

### 其他构造器

```c
// copy all the elements from another container
C<T> c(o);
```

```c
// copy all the elements from the given range denoted by iterator b and e
C<T> c(b, e);
```

```c
// n elements with value-initialized
C<T> c(n);
```

```c
// n elements with given value
C<T> c(n, v);
```

### 容器元素类型限制

- 支持赋值操作
  
- 支持复制操作

由于容器支持上述操作，故容器类型也可以作为容器的元素类型，即支持 *容器嵌套*。

```c
    vector<vector<string>> lines;
```

## 迭代器和迭代器范围

**标准容器迭代器操作**

许多容器迭代器对象 *继承了相同的接口*，因此他们能够 *支持相同的操作*。

- 解引用操作

    ```c
    // 返回当前迭代器指向容器元素的引用
    *iter

    // this equals to （*iter).mem
    iter->mem
    ```

- 自增/自减操作

    ```c
    // 返回指向下一个容器元素的迭代器
    ++iter;
    iter++;

    // 返回指向上一个容器元素的迭代器
    --iter;
    iter--;
    ```

- 相等/不等操作

    ```c
    // 判断两个迭代器是否指向相同的容器元素
    iter1 == iter2
    iter1 != iter2
    ```

**vector & deque 迭代器支持的其他操作**

- 加法 / 减法操作
    ```c
    // 等价于 n 次自增 / 自减操作
    iter + n
    iter - n

    // 前提：两个迭代器指向同一个容器中的元素
    // 返回两个迭代器之间元素的数量
    item2 - item1
    ```

- 复合赋值操作

    ```c
    iter += n;
    iter -= n;
    ```

- 比较操作

    ```c
    >, >=, <, <=
    ```

### 迭代器区间（Iterator Ranges）

迭代器区间由指向两个容器元素的迭代器构成，这两个迭代器分别表示 *迭代器区间的开始和结束*，一般使用 `first` 和 `last` 或者 `beg` 和 `end` 来分别表示。

迭代器区间表示的容器元素范围是 *左开右闭*，即 `[first, last)`。

**构成迭代器区间的要求**

- 指向 *同一容器中的元素* 或者 *容器最后一个元素的下一个位置（One past the end）*。

- 如果两个迭代器指向不同的元素，那么 `first` 迭代器能够通过不断地自增运算到达 `last` 迭代器，也即 *`last` 迭代器不能位于 `first` 迭代器之前*。 

注：编译器本身并不会强制这种约束，这只是一种约定俗成的规范。

**为什么 `last` 迭代器指向区间元素的下一个位置？**

- 当 `first == last` 时，区间为空。

- 当 `first != last` 时，该区间内至少包含一个元素且 `first` 指向该区间内的第一个元素，*我们可以通过自增 `first` 迭代器到达 `second` 迭代器*。

注： 如果考虑迭代器区间为 `[first, last]` 时，`first` 迭代器移动到 `last` 迭代器的下一个位置时停止，即 `first == ++last`。因为要涉及额外的自增操作，这种实现效率更低。

```c
// [first, last)
while (first != last) {
    ...
    ++first;
}

// [first, last]
while (first != ++last) {
    ...
    ++first;
}
```

**迭代器失效** 是因为对容器的插入、删除操作导致 *迭代器不再指向原先的容器元素*，有时并不是所有的迭代器都会失效，具体情况与容器类型相关。

## 序列容器操作

### `begin` 和 `end` 函数成员

```c
// 返回指向第一个容器元素的迭代器
c.begin();

// 返回指向最后一个容器元素下一个位置的迭代器
c.end();

// 逆序迭代器
c.rbegin();
c.rend();
```

`begin` 和 `end` 均有两种 non-const 和 const 版本的成员函数。

前者迭代器返回类型分别为 `iterator` 和 `reverse_iterator`，而后者以 `const_` 作为前缀。

### 添加元素

```c
string text_word;

while (cin >> text_word) {
    c.push_back(text_word);
}
```

容器元素具有 “值语义”，也即 *向容器中添加的是元素的复制*。

```c
// 在容器末尾插入元素 t
c.push_back(t);

// 在容器开始插入元素 t
c.push_front(t);

// 在迭代器 iter 指向元素之前插入元素 t 并返回指向新插入元素位置的迭代器
c.insert(iter, t);

// 在迭代器 iter 指向元素之前插入 n 个元素 t
c.insert(iter, n, t);

// 在迭代器 iter 指向元素之前插入由 [b, e) 确定的迭代器区间内的元素
c.insert(iter, b, e);
```

**为什么 `iter` 迭代器前一个位置插入元素？**

`iter` 迭代器能够指向 *容器任意位置*，包括容器中最有一个元素的下一个位置。

### 关系操作

关系操作允许我们比较两个容器，前提是两个容器具有相同的类型（包括元素类型）。

```c
    // allowed
    vector<int> vs. vector<int> 

    // not allowed
    vector<int> vs. list<int>
    vector<int> vs. vector<double>
```

容器之间的关系操作是基于容器元素进行且遵循以下规则：

0. *容器元素类型本身需支持相应的关系操作*。

1. 如果容器 size 相同且所有元素是相等的，那么两个容器相等；否则，容器不相等。

2. 如果容器 size 不同且较短容器元素与较长容器的对应元素相等，那么较短容器小于较长容器。

3. 如果非 1. 和 2. 情况，比较结果由两个容器首个不相等元素决定。


```c
ivec1: 1 3 5 7 9 12
ivec2: 0 2 4 6 8 10 12
ivec3: 1 3 9
ivec4: 1 3 5 7
ivec5: 1 3 5 7 9 12

// Case 1
// true
ivec1 == ivec5 
// false
ivec1 == ivec4

// Case 2
// true
ivec1 > ivec3

// Case 3
// false
ivec1 < ivec2
// true
ivec2 < ivec1
```

### 容器大小操作

```c
// 容器中元素的数量
c.size();

// 容器最大所能容纳元素的数量
c.max_size();

// 判断容器是否为空
c.empty();

// 重新调整容器的大小为 n
// 如果 n < c.size(), 多余元素将被丢弃；
// 如果 n > c.size(), 新添加元素将值初始化；
c.resize(n);

// 重新调整容器的大小为 n 
// 新添加元素使用 t 进行初始化
c.resize(n, t);
```

### 访问元素

```c
// 返回容器第一个元素
c.front()

// 返回容器最后一个元素
c.back()

// 返回索引位置为 n 的元素
// 仅适用于 vector 以及 deque
// list 不支持随机访问
c[n]

// 异常安全
// 如果 n 为非法索引，则抛出 `out_of_range` 异常；
c.at(n)
```

### 移除元素

```c
// 移除 iter 指向的元素
// 返回指向移除元素下一个位置元素的迭代器
c.erase(iter);

// 移除由 [beg, end) 确定的迭代器区间内的所有元素
// 返回指向       迭代器区间内最后一个元素的下一个位置元素的迭代器
c.erase(beg, end);

// 移除所有元素
// 等价于 c.erase(c.begin(), c.end())
c.clear();

// 移除容器最后一个元素
c.pop_back();

// 移除容器第一个元素
c.pop_front();
```

### 赋值与交换
                                                                                                                                                          
```c
c1 = c2;

// 等价操作
c1.erase(c1.begin(),  c1.end());
c1.insert(c2.begin(), c2.end());
```

```c
// 将 c1 中的元素替换为由 [beg, end) 确定的迭代器区间内的元素
// 因为删除操作会使得 beg 和 end 迭代器失效，所以 beg 和 end 不能指向容器 c1 中的元素。
c1.assign(beg, end);

// 将 c1 中的元素替换为 n 个值为 t 的元素
c1.assign(n, t);

// 交换两个容器内的元素（容器类型及其元素类型必须相同）
// swap 操作并涉及删除和移动操作
// swap 不会使得迭代器失效，但迭代器指向元素位于不同的容器
c1.swap(c2);
```

## 扩容机制

由于序列容器的存储空间是连续的，当容器无法容纳新元素时，需要为容器重新分配新的存储空间，即扩容。

### `capacity` 和 `reserve` 函数成员

**`size` 和 `capacity` 的区别**

前者返回当前容器内元素的数量，而后者返回当前容器的容量（即在不扩容的情况下的最大元素数量）。

```c
vector<int> ivec;
// ivec size: : 0, capacity: 24
cout << "ivec size: " << ivec.size() << ", capacity: " << ivec.capacity() << endl;

for (int i = 0; i < 24; ++i>) {
    ivec.push_back(i);
}
// ivec size: : 24, capacity: 32
cout << "ivec size: " << ivec.size() << ", capacity: " << ivec.capacity() << endl; 


// 将 capacity 设置为 50 (可能更大)
ivec.reserve(50);
// ivec size: : 24, capacity: 50
cout << "ivec size: " << ivec.size() << ", capacity: " << ivec.capacity() << endl;

while (ivec.size() != ivec.capacity()) {
    ivec.push_back(0);
}
// ivec size: : 50, capacity: 50
cout << "ivec size: " << ivec.size() << ", capacity: " << ivec.capacity() << endl;


ivec.push_back(42);
// ivec size: : 51, capacity: 100
cout << "ivec size: " << ivec.size() << ", capacity: " << ivec.capacity() << endl;
```

容器的扩容机制按照一定规则增长内存空间，通常为指数级增长。

## 容器适配器

容器适配器对已有的容器类型进行封装，使其在行为上与底层容器类型不同，但行为的实现受限于底层容器类型。

```c
#include <stack>
#include <queue>
#include <priority_queue>

// stack, queue <- deque
// priority_queue <- vector
```

**初始化**
```c
// 默认构造
C<T> c;

// 复制构造
C<T> c(o);
```

**关系操作**

```c
// 底层本身支持关系操作，故适配器也可以支持相应操作
>, >=, ==, !=, <=, <
```

### `stack` 适配器

```c
// 判断栈是否为空
s.empty();

// 返回栈内元素的数量
s.size();

// 移除栈顶元素但不返回相应元素
s.pop();

// 返回栈顶元素
s.top();

// 在栈顶内添加元素
s.push(item);
```

### `queue` 和 `priority_queue` 适配器

```c
// 判断队列是否为空
q.empty();

// 返回队列内元素的数量
q.size();

// 移除队首元素
q.pop();

// 返回队首元素
q.front();

// 返回队尾元素
q.back();

// 返回具有最高优先级的元素
q.top();

// 插入元素
q.push(item);

```