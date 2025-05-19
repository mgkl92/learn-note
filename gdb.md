# GBD Basic Usage

## 调试程序

```shell
gdb <program name>
```

## 基本命令

现假设，我们有如下程序：

```c
#include <stdio.h>

int main(int argc, const char * argv[]) {
        int arr[4] = {1, 2, 3, 4};

        for (size_t i = 0; i < 4; ++i) {
                printf("%d\n", arr[i]);
        }

        return 0;
}
```

### 运行程序 `run[r]`

```shell
（gdb）r

# Output
Starting program: /home/mgkl/workspace/cpp_projects/gbd_examples/a.out 
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
1
2
3
4
Hello, World!
[Inferior 1 (process 24958) exited normally]
```

### 添加断点 `break[b]`

```shell
# 在函数出添加断点
（gdb）break <function name>
（gdb）break main

# Output
Breakpoint 3 at 0x55555555517c: file test.c, line 3.

# 在某行添加断点
（gdb）break <line number>
（gdb）break 6

# Output
Breakpoint 2 at 0x5555555551a7: file test.c, line 6.

# 查看断点
（gdb）info b

# Output
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x000055555555517c in main(int, char const**) at test.c:3
2       breakpoint     keep y   0x00005555555551a7 in main(int, char const**) at test.c:6
```

### 查看变量内容 `print[p]`

```shell

# 打印数组 arr 的内容
（gdb）p arr

# Output
$7 = {0, 0, 0, 0}

# 打印变量地址
（gdb）p &arr

# Output
$11 = (int (*)[4]) 0x7fffffffd700

（gdb）p &arr[0]

# Output
$9 = (int *) 0x7fffffffd700

（gdb）p &arr[1]
# Output
$10 = (int *) 0x7fffffffd704
```

### 进入函数调用 `step[s]`

现假设，我们有如下程序：

```c
#include <stdio.h>

int main(int argc, const char * argv[]) {
        int arr[4] = {1, 2, 3, 4};

        for (size_t i = 0; i < 4; ++i) {
                printf("%d\n", arr[i]);
        }

        return 0;
}
```

```shell
# 在 hello 函数调用出添加断点
（gbd）b 14

# Output
Breakpoint 1 at 0x1215: file test.c, line 14.

# 运行程序
（gbd）r

# Output
Starting program: /home/mgkl/workspace/cpp_projects/gbd_examples/a.out 
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
1
2
3
4

Breakpoint 1, main (argc=1, argv=0x7fffffffd838) at test.c:14
14              hello();

# 进入 hello 函数
（gbd）s

# Output
hello () at test.c:4
4               printf("Hello, World!\n");

# 继续执行程序
（gbd）n

# Output
Hello, World!
5       }
```

### 日志功能

```shell
（gdb）set logging enable on

# Output
Copying output to gdb.txt.
Copying debug output to gdb.txt.
```

### 设置观察点 `watch`

```shell
# 在 for 循环入口设置断点并观察变量 i 
（gdb）b 10
（gdb）wathc i

# Output
Hardware watchpoint 2: i
11                      printf("%d\n", arr[i]);
10              for (size_t i = 0; i < 4; ++i) {

Hardware watchpoint 2: i

Old value = 0
New value = 1
0x000055555555520e in main (argc=1, argv=0x7fffffffda08) at test.c:10
10              for (size_t i = 0; i < 4; ++i) {

# 查看观察点
（gdb）info watch

# Output
Num     Type           Disp Enb Address            What
2       hw watchpoint  keep y   
```

### 使用 core 文件调试程序

```shell
gdb <binary program> <core file>
```

### 调试运行中的程序

现在假设，我有如下文件

```c
#include <stdio.h>

void hello() {
        printf("Hello, World!\n");
}

void test() {
}

void test1() {
        int i = 0;
        ++i;
}

int main(int argc, const char * argv[]) {
        int arr[4] = {1, 2, 3, 4};

        for (size_t i = 0; i < 4; ++i) {
                printf("%d\n", arr[i]);
        }

        hello();

        // some bug here
        // int *temp = NULL;
        // *temp = 10;

        for(;;) {
                test();
                test1();
        }
 
        return 0;
}
```

```shell
gdb -p <pid>
```

```shell
# 在新的终端运行程序
./a.out &

# Output
[1] 22622

# 调试 pid 为 22622 的程序
gdb -p 22622

# Output
...
Attaching to process 22622
Reading symbols from /home/mgkl/workspace/cpp_projects/gdb_examples/a.out...
Reading symbols from /lib/x86_64-linux-gnu/libc.so.6...
Reading symbols from /usr/lib/debug/.build-id/cd/410b710f0f094c6832edd95931006d883af48e.debug...
Reading symbols from /lib64/ld-linux-x86-64.so.2...
Reading symbols from /usr/lib/debug/.build-id/e4/de036b19e4768e7591b596c4be9f9015f2d28a.debug...
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
test1 () at test.c:13
13      }

# 逐步查看程序执行情况
（gdb）n

# Output
30                      test1();
```
