# Makefile 基础

### （构建）规则、目标和依赖

规则由目标（Targets）、依赖（prerequisites / dependencies）和命令三部分组成；其中，目标为构建规则需要生成的文件、依赖为生成目标文件所需要的依赖文件，而命令指导构建系统如何使用依赖文件生成目标文件。

其基本语法如下：

```shell
    targets : prerequisites
        command(s)
```

### Make 的本质

首先，应该明白，**目标 ≠ 文件**，且当执行构建规则时，其最终期望的结果是生成与目标同名的文件。

构建规则在以下两种情况尝试运行规则中的命令去构建目标文件：

1. 构建目标文件尚未生成；
2. **构建目标依赖文件在目标文件生成后发生变更（构建系统对比目标文件与依赖文件的时间戳）。**

### 变量

Make 中，变量只能为字符串（即仅支持字符串类型）；且 Make 并不识别引号（单引号或双引号），即引号对 Make 无特殊含义。

其基本语法为

```shell
    # 定义变量
    var := val

    # 引用变量
    $(var) or ${var}
```

1. **``=`` 递归赋值**
    
    当且仅当命令执行时，定义该变量；

2. **``:=`` 展开赋值**

    该方式允许你追加变量；且在变量定义时，直接对变量展开。

    ```shell
        one = hello
        
        # hello -> hello there
        one := ${one} there

        # `one = ${one} there` will cause an error.

        all: 
            echo $(one)
    ```

3. **``+=`` 追加赋值**

4. **``?=`` 仅当变量未定义时定义该变量**

> 注：当引用未定义变量时，其值为空。

### 通配符

1. ``*`` 用于搜索文件系统以匹配文件名称；但未匹配到任何文件名时，该通配符会保留；因此，``*`` 一般不直接使用，通过与 ``wildcard`` 函数组合使用。

    如 

    ```shell
        # 匹配所有 .c 文件
        $(wildcard *.c)
    ```

2. ``%`` 当处于 **匹配模式** 时，该通配符会将匹配到的一个或多个字符串作为 *词干（stem）*；当处于 **替换模式** 时，该通配符会将匹配位置替换为词干。

### 自动变量

```shell
    # 目标名称（当有多个目标时，匹配当前生成目标）
    $@

    # 所有时间戳比目标文件新的依赖文件
    $?

    # 所有依赖文件
    $^

    # 第一个依赖文件
    $<
```

### 魔法规则

1. **隐式规则** 

   当需要相应依赖时，Make 将自动执行 ``$(CC) -c $(CPPFLAGS) $(CFLAGS) $^ -o $@`` 或 ``$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $^ -o $@`` 将相应的 `n.c` 文件或 `n.cc / n.cpp` 文件生成其相应的目标文件 ``n.o``；此外，Make 也可以自动的执行 ``$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@`` 为你将单个文件 ``n.o`` 链接到 ``n``。
    
2. **静态模式规则**
    
    基本语法为

    ```shell
        targets...: target-pattern: prereq-patterns ...
            commands
    ```
    
    上述规则会为所有的目标文件生成构建规则并为匹配的目标文件生成相应的依赖文件。
    
    现假设有如下 Makefile 文件

    ```shell
        objects = foo.o bar.o all.result
        all: $(objects)
            $(CC) $^ -o all

        $(objects): %.o: %.c
            $(CC) -c $^ -o $@

        all.c:
            echo "int main() { return 0; }" > all.c

        %.c:
            touch $@

        clean:
            rm -f *.c *.o all

        # Outputs
        #    Makefile:8: target 'all.result' doesn't match the target pattern
        #    touch foo.c
        #    cc -c foo.c -o foo.o
        #    touch bar.c
        #    cc -c bar.c -o bar.o
        #    cc -c  -o all.result
        #    cc: fatal error: no input files
        #    compilation terminated.
        #    make: *** [Makefile:9: all.result] Error 1
    ```

3. **过滤函数 ``filter``**

    上述问题可以先对目标文件使用过滤函数 ``filter`` 解决。

    基本语法为

    ```shell
        $(filter %.o,$(obj_files)): %.o: %.c
            $(CC) -c $^ -o $@
    ```
4. **模式规则**

    该规则相较静态模式规则和过滤函数更为通用。

    ```shell
        %.o : %.c
		$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@
    ```

5. **双冒号（::）规则**

    该规则允许为同一目标定义多个规则。

    ```shell
        all: blah

        blah::
            echo "hello"

        blah::
            echo "hello again"
    ```

### 命令与执行

1. 默认情况下，Make 会在执行规则中的命令前将其输出到控制台。在命令前使用 ``@`` 可以禁止该行为（仅对当前命令有效）；使用 ``make`` 的 ``-s`` 选项会为每条命令的执行添加 ``@``；

2. **每条命令的执行都使用新的 ``shell``**；

3. ``$`` 在 Make 中有特殊含义，故可使用 ``$$`` 作为 ``$`` 的转译；

4. 使用 ``$(MAKE)`` 可以递归的执行构建过程，如：

    ```shell
        new_contents = "hello:\n\ttouch inside_file"
        all:
            mkdir -p subdir
            printf $(new_contents) | sed -e 's/^ //' > subdir/makefile
            cd subdir && $(MAKE)

        clean:
            rm -rf subdir
    ```

5. ``export``

    当 Make 构建时，它在其当前环境变量的基础上创建 Make 变量，如：

    ```shell
        # Run this with "export shell_env_var='I am an environment variable'; make"
        all:
            # Print out the Shell variable
            echo $$shell_env_var

            # Print out the Make variable
            echo $(shell_env_var)
    ```

    此外，你可以使用 ``export`` 命令将 Make 变量导出到环境变量中以供当前 ``shell`` 使用；因此，Make 的这种特性使得其能够调用子进程执行子构建时共享某些变量，如

    ```shell
        shell_env_var=Shell env var, created inside of Make
        export shell_env_var
        all:
            echo $(shell_env_var)
            echo $$shell_env_var
    ```

    ``.EXPORT_ALL_VARIABLES`` 会自动为你导出所有 Make 变量。

6. **``define`` 与命令列表**

    ``define`` 允许你定义变量来存储命令列表（多条命令）。

    ```shell
        one = export blah="I was set!"; echo $$blah

        define two
        export blah="I was set!"
        echo $$blah
        endef

        all: 
            @echo "This prints 'I was set'"
            @$(one)
            @echo "This does not print 'I was set' because each command runs in a separate shell"
            @$(two)
    ```

### 条件控制
    
 1. ``ifeq...else...endif``

    ```shell
        foo = ok

        all:
        ifeq ($(foo), ok)
            echo "foo equals ok"
        else
            echo "nope"
        endif
    ```

 2. ``ifneq...endif``

### 函数

函数调用形如 ``$(fn, args)`` 或 ``${fn, args}``

```shell
    # 所有的匹配字符均会被替换！！！
    bar := ${subst not,"totally", "I am not superman"}
    all: 
	    @echo $(bar)
```

1. **字符替换 ``$(patsubst pattern,replacement,text)``**
    ```shell
        foo := a.o b.o l.a c.o
        one := $(patsubst %.o,%.c,$(foo))

        # Result -> one := a.c b.c l.a c.c
    ```

2. **循环 ``$(foreach var,list,text)``**

    ```shell
        foo := who are you
        bar := $(foreach wrd,$(foo),$(wrd)!)

         # Result -> foo := who! are! you!
    ```

3. **空字符串判断 ``${if arg, then, else}``**

    ```shell
        foo := $(if this-is-not-empty,then!,else!)
        empty :=
        bar := $(if $(empty),then!,else!)

        all:
            @echo $(foo)
            @echo $(bar)
        
        # Outputs
        #   then!
        #   else!
    ```

4. **过滤函数 ``${filter pattern, arg}``**

    ```shell
        obj_files = foo.result bar.o lose.o
        filtered_files = $(filter %.o,$(obj_files))

        all:
            @echo $(filtered_files)

        # Result -> filtered_files = bar.o lose.o
    ```

5. **自定义函数调用 ${call fn_name, arg(s)}**

    ```shell
        sweet_new_fn = Variable Name: $(0) First: $(1) Second: $(2) Empty Variable: $(3)
        all:
            @echo $(call sweet_new_fn, go, tigers)

        # Outputs "Variable Name: sweet_new_fn First: go Second: tigers Empty Variable:"
    ```

6. **执行 ``shell`` 命令 ``${shell command(s)}``**

    ```shell
        all: 
            @echo $(shell ls -la) # Very ugly because the newlines are gone!
    ```

### 其他特性

1. **``include`` 指令** 允许 Make 读取多个 makefile。

    ```shell
        include filenames...
    ```

2. **``vpath`` 指令**

    默认情况下，Make 会在当前工作目录查找依赖文件；**而 vpath 允许指定在指定目录查找依赖文件**。

   ```shell
        # Syntax
        vpath <pattern> <directories>

        # Example
        vpath %.h ../headers ../other-directory
    ```

3. ``.PHONY`` 伪目标
    
    当 Make 中存在与伪目标同名的目标文件时，该伪目标将失效；因为伪目标没有依赖文件，故其相应命令将永远得不到执行。
    
    Make 提供了 ``.PHONY`` 以避免上述问题。

    ```shell
        .PHONY: clean
        clean:
            rm -f some_file
            rm -f clean
    ```

4. ``.DELETE_ON_ERROR``

    该特性使得 Make 在某条构建规则中的某个命令失败时（非零返回值），将删除相应的目标文件。

    ```shell
        .DELETE_ON_ERROR:
        all: one

        one:
            touch one
            false

        # Outputs
        #    touch one
        #    false
        #    make: *** [Makefile:6: one] Error 1
        #    make: *** Deleting file 'one'
    ```