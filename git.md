# Git Immersion 学习

[教程连接](https://gitimmersion.com/index.html)

## 1 设置用户名和邮箱

```shell
# 设置用户名为 mgkl92
git config --global user.name "mgkl92"

# 设置邮箱为 zst_lff@126.com
git config --global user.email "zst_lff@126.com"
```

## 3 创建一个项目

```shell
# 初始当前目录为 git 项目
git init

# Output
Initialized empty Git repository in /home/mgkl/Desktop/git_tutorial/hello/.git/
```

```cpp
// 添加 hello.rb 文件到暂存区
git add hello.rb
```

```cpp
// 进行一次提交
git commit -m "First Commit"

# Output
[master (root-commit) 296786b] First Commit
 1 file changed, 1 insertion(+)
 create mode 100644 hello.rb
```
## 4 检查仓库状态

```shell
# 检查仓库当前状态
git status

# Output
On branch master
nothing to commit, working tree clean
```

## 5 文件变更

```shell
# 将 hello.rb 内容修改为
puts "Hello, #{ARGV.first}!"
```

```shell
# 检查修改 hello.rb 内容后仓库的状态
git status

# Output
On branch master
Changes not staged for commit:
  (use "git add <file>..." to update what will be committed)
  (use "git checkout -- <file>..." to discard changes in working directory)

        modified:   hello.rb

no changes added to commit (use "git add" and/or "git commit -a")
```

## 6 暂存变更

```shell
# 将对 hello.rb 的修改添加到暂存区
git add hello.rb

# 检查将 hello.rb 添加到暂存区后的仓库状态
git status

# Output
On branch master
Changes to be committed:
  (use "git reset HEAD <file>..." to unstage)

        modified:   hello.rb
```

## 8 提交变更

使用无参的 `git commit` 命令会交互的弹出默认的文本编辑器，可以添加任意对本次提交内容的注释。

```shell
Using ARGV
# Please enter the commit message for your changes. Lines starting
# with '#' will be ignored, and an empty message aborts the commit.
# On branch master
# Changes to be committed:
#   (use "git reset HEAD <file>..." to unstage)
#
#	modified:   hello.rb
```

```shell
# 提交对 hello.rb 的变更
git commit

# Output
[master 55b8500] Using ARGV
 1 file changed, 1 insertion(+), 1 deletion(-)
```

## 7 暂存与提交分离的哲学

额外的暂存过程允许我们能够仅在必要的时候进行源控制，这使得我们可以持续的变更我们工作目录中的内容。

现在假设我们有三个文件 a.rb, b.rb, c.rb，其中 a.rb, b.rb 内容相关，而 c.rb 与其它两个文件逻辑无关。因此，我们可以先提交对 a.rb 和 b.rb 内容修改；然后，再提交对 c.rb 内容的修改。

```shell
# 提交 a.rb 以及 b.rb
git add a.rb 
git add b.rb

git commit -m "Changes for a and b"

# 提交 c.rb
git add c.rb

git commit -m "Unrelated change to c"
```

# 9 变更并非文件

Git 聚焦于文件的变更而非文件本身，当你使用 `git add file` 命令时，并非告知 git 将该文件添加到仓库，而是令 git 关注该将要提交文件的当前状态。

首先，假设我们对 hello.rb 作如下变更：

```shell
# hello.rb
name = ARGV.first || "World"

puts "Hello, #{name}!"
```

```shell
# 将变更添加到暂存区
git add hello.rb
```

然后，我们再次对 hello.rb 作如下变更：
```shell
# hello.rb
# Default is "World"
name = ARGV.first || "World"

puts "Hello, #{name}!"
```

```shell
# 检查仓库的当前状态
git status 

# Output
On branch master
Changes to be committed:
  (use "git reset HEAD <file>..." to unstage)

        modified:   hello.rb

Changes not staged for commit:
  (use "git add <file>..." to update what will be committed)
  (use "git checkout -- <file>..." to discard changes in working directory)

        modified:   hello.rb
```

此时，我们可以

```shell
# Output
On branch master
Changes to be committed:
  (use "git reset HEAD <file>..." to unstage)

        modified:   hello.rb

Changes not staged for commit:
  (use "git add <file>..." to update what will be committed)
  (use "git checkout -- <file>..." to discard changes in working directory)

        modified:   hello.rb
```

此时，hello.rb 出现了两次：第一次出现的 hello.rb 会进入下一次提交，而未进入暂存区的 hello.rb 并不会进入下一次提交。

```shell
# 提交第一次出现的 hello.rb
git commit -m "Added a default value"

# 检查提交后的仓库状态
git status

# Output
On branch master
Changes not staged for commit:
  (use "git add <file>..." to update what will be committed)
  (use "git checkout -- <file>..." to discard changes in working directory)

        modified:   hello.rb

no changes added to commit (use "git add" and/or "git commit -a")

# 暂存第二次出现的 hello.rb，然后提交
git add hello.rb

git commit -m "Added a comment"
```

# 10 提交历史

```shell
# 列出当前仓库的已提交的记录
git log

# Output
commit 034d418e709579f1b8142fef8dcb07ff577aea1f (HEAD -> master)
Author: mgkl92 <zst_lff@126.com>
Date:   Mon May 5 21:15:52 2025 +0800

    Added a comment

commit 2c1e42dc3788479fb042d149fc97c9c1c5bf9ea9
Author: mgkl92 <zst_lff@126.com>
Date:   Mon May 5 21:12:50 2025 +0800
Author: mgkl92 <zst_lff@126.com>
Date:   Mon May 5 20:54:06 2025 +0800

    Changes for a and b

commit 55b85002f786a071843ce716802db761d356d314
Author: mgkl92 <zst_lff@126.com>
Date:   Mon May 5 20:29:14 2025 +0800

    Using ARGV

commit 296786b086af9ae96882b7ae65d01c50508bdd54
Author: mgkl92 <zst_lff@126.com>
Date:   Mon May 5 20:13:33 2025 +0800

    First Commit

# 一行历史
git log --pretty=oneline

# Output
034d418e709579f1b8142fef8dcb07ff577aea1f (HEAD -> master) Added a comment
2c1e42dc3788479fb042d149fc97c9c1c5bf9ea9 Added a default value
de82ecc3201206c8690d9a71301f0e9c91f67940 Unrelated change to c
74cf2db98a4b2b11866fe967701f01ea3abad9cb Changes for a and b
55b85002f786a071843ce716802db761d356d314 Using ARGV
296786b086af9ae96882b7ae65d01c50508bdd54 First Commit
```

**控制历史输出记录**

```shell
# 最多输出两条提交记录
git log --pretty=oneline --max-count=2

# Output
034d418e709579f1b8142fef8dcb07ff577aea1f (HEAD -> master) Added a comment
2c1e42dc3788479fb042d149fc97c9c1c5bf9ea9 Added a default value

# 输出五分钟内的提交记录
git log --pretty=oneline --since='5 minutes ago'

# 输出五分钟前的提交记录
git log --pretty=oneline --until='5 minutes ago'

# Output
034d418e709579f1b8142fef8dcb07ff577aea1f (HEAD -> master) Added a comment
2c1e42dc3788479fb042d149fc97c9c1c5bf9ea9 Added a default value
de82ecc3201206c8690d9a71301f0e9c91f67940 Unrelated change to c
74cf2db98a4b2b11866fe967701f01ea3abad9cb Changes for a and b
55b85002f786a071843ce716802db761d356d314 Using ARGV
296786b086af9ae96882b7ae65d01c50508bdd54 First Commit

# 输出指定用户的提交记录
git log --pretty=oneline --author=mgkl92

# Output
034d418e709579f1b8142fef8dcb07ff577aea1f (HEAD -> master) Added a comment
2c1e42dc3788479fb042d149fc97c9c1c5bf9ea9 Added a default value
de82ecc3201206c8690d9a71301f0e9c91f67940 Unrelated change to c
74cf2db98a4b2b11866fe967701f01ea3abad9cb Changes for a and b
55b85002f786a071843ce716802db761d356d314 Using ARGV
296786b086af9ae96882b7ae65d01c50508bdd54 First Commit

# 输出所有提交记录
# 默认，git log 仅输出当前分支上的提交记录
git log --pretty=oneline --all
```

**推荐输出格式**

```shell

git log --pretty=format:'%h %ad | %s%d [%an]' --graph --date=short

# Output
* 034d418 2025-05-05 | Added a comment (HEAD -> master) [mgkl92]
* 2c1e42d 2025-05-05 | Added a default value [mgkl92]
* de82ecc 2025-05-05 | Unrelated change to c [mgkl92]
* 74cf2db 2025-05-05 | Changes for a and b [mgkl92]
* 55b8500 2025-05-05 | Using ARGV [mgkl92]
* 296786b 2025-05-05 | First Commit [mgkl92]

# 符号解释
# %h 提交记录的哈希值
# %d 提交记录的装饰（分支或者标签）
# %ad 记录提交日期
# %an 记录提交用户
# %s 提交记录注释
# --graph
# --date=short
```

# 11 命令别名

Git 别名允许我们为常用命令提供缩写。

以 Linux 操作系统为例，我们可以在 $HOME 目录下的 .gitconfig 文件添加如下内容：

```shell
[alias]
  co = checkout
  ci = commit
  st = status
  br = branch
  hist = log --pretty=format:'%h %ad | %s%d [%an]' --graph --date=short
```

# 12 获取旧版本

我们可以根据提交记录的哈希值，将文件切换到旧版本。

```shell
# 查看提交记录
git hist

# Output
* 034d418 2025-05-05 | Added a comment (HEAD -> master) [mgkl92]
* 2c1e42d 2025-05-05 | Added a default value [mgkl92]
* de82ecc 2025-05-05 | Unrelated change to c [mgkl92]
* 74cf2db 2025-05-05 | Changes for a and b [mgkl92]
* 55b8500 2025-05-05 | Using ARGV [mgkl92]
* 296786b 2025-05-05 | First Commit [mgkl92]

# 切换到旧版本
# 进入分离头状态
git checkout 2c1e42d

# Output
Note: checking out '2c1e42d'.

You are in 'detached HEAD' state. You can look around, make experimental
changes and commit them, and you can discard any commits you make in this
state without impacting any branches by performing another checkout.

If you want to create a new branch to retain commits you create, you may
do so (now or later) by using -b with the checkout command again. Example:

  git checkout -b <new-branch-name>

HEAD is now at 2c1e42d Added a default value

# 对比使用和不使用 --all 选项 git log 的输出
# 检查历史记录
git hist

# Output
* 2c1e42d 2025-05-05 | Added a default value (HEAD) [mgkl92]
* de82ecc 2025-05-05 | Unrelated change to c [mgkl92]
* 74cf2db 2025-05-05 | Changes for a and b [mgkl92]
* 55b8500 2025-05-05 | Using ARGV [mgkl92]
* 296786b 2025-05-05 | First Commit [mgkl92]

# 检查历史记录
git hist --all

# Output
* 034d418 2025-05-05 | Added a comment (master) [mgkl92]
* 2c1e42d 2025-05-05 | Added a default value (HEAD) [mgkl92]
* de82ecc 2025-05-05 | Unrelated change to c [mgkl92]
* 74cf2db 2025-05-05 | Changes for a and b [mgkl92]
* 55b8500 2025-05-05 | Using ARGV [mgkl92]
* 296786b 2025-05-05 | First Commit [mgkl92]

# 切换到 master 分支的最新版本
git checkout master
```

**分离头指针**

![detached-HEAD](/pics/detached-HEAD.jpg)

默认情况，Git 使用 HEAD 指针记录当前所处分支的最新提交记录；而当你检出某一个提交记录时，HEAD 指针会指向该提交记录，不再指向任何具体的分支。非分离头状态下，HEAD 指针不是指向就是 master 分支就是指向 dev 分支；但 Git 允许你检出到 dev 分支独有的提交记录 commit4 上，也允许你检出到 master 分支和 dev 分支的公共提交记录 commit2 上（从指向 commit2 更有助于我们理解分离头状态）。

# 13 标记版本

```shell
# 为当前提交记录添加标签 v1
git tag v1

# 检查历史记录
git hist

# Output
* 2c1e42d 2025-05-05 | Added a default value (HEAD, tag: v1) [mgkl92]
* de82ecc 2025-05-05 | Unrelated change to c [mgkl92]
* 74cf2db 2025-05-05 | Changes for a and b [mgkl92]
* 55b8500 2025-05-05 | Using ARGV [mgkl92]
* 296786b 2025-05-05 | First Commit [mgkl92]

# 通过标签 v1 检出提交记录
git tag v1

# Output
Note: checking out 'v1'.

You are in 'detached HEAD' state. You can look around, make experimental
changes and commit them, and you can discard any commits you make in this
state without impacting any branches by performing another checkout.

If you want to create a new branch to retain commits you create, you may
do so (now or later) by using -b with the checkout command again. Example:

  git checkout -b <new-branch-name>

HEAD is now at 2c1e42d Added a default value

# 检视已有标签
git tag

# Output
v1
v1-beta
```

# 14 撤销本地变更（暂存前）

现在假设我们对 hello.rb 文件有如下更改尚未添加到暂存区：

```txt
# This is a bad comment. We want to revert it.
# Default is "World"
name = ARGV.first || "World"

puts "Hello, #{name}!"
```

```shell
# 检查当前仓库状态
git hist

# Output
On branch master
Changes not staged for commit:
  (use "git add <file>..." to update what will be committed)
  (use "git checkout -- <file>..." to discard changes in working directory)

        modified:   hello.rb

no changes added to commit (use "git add" and/or "git commit -a")

# 检出当前分支上的 hello.rb
git checkout hello.rb

# 检查当前仓库状态
On branch master
nothing to commit, working tree clean
```

检出某个文件为当前提交记录中的版本，这会使得我们在工作区中对该文件尚未添加至暂存区的修改丢失！
所以，现在 hello.rb 文中的内容如下：

```txt
# Default is "World"
name = ARGV.first || "World"

puts "Hello, #{name}!"
```

# 15 撤销暂存区中的修改（提交前）

现在假设，我们对 hello.rb 有如下修改且已添加至暂存区：

```txt
# This is an unwanted but staged comment
name = ARGV.first || "World"

puts "Hello, #{name}!"
```

```shell
# 检查当前仓库状态
git st

# Output
On branch master
Changes to be committed:
  (use "git reset HEAD <file>..." to unstage)

        modified:   hello.rb

# 复位暂存区
git reset HEAD hello.rb

# Output
Unstaged changes after reset:
M       hello.rb
```

使用 git reset 命令并不会像使用 git checkout 命令那样导致我们工作区中对文件的变更。
这也就是说，我们的 hello.rb 文件的内容如下：

```txt
# This is an unwanted but staged comment
name = ARGV.first || "World"

puts "Hello, #{name}!"
```

但值得注意的是，当使用 git checkout 命令检出文件时并不会影响我们已经添加到暂存区中的内容；而且当你将修改添加到暂存区后，使用再进行检出操作，此时并不会影响已提交至暂存区中的变更（与暂存区中的内容保持一致）。

# 16 撤销提交记录

现在假设，我们对 hello.rb 进行了如下修改并进行了提交：

```txt
# This is an unwanted but committed change
name = ARGV.first || "World"

puts "Hello, #{name}!"
```

```shell
# 检查历史记录
git hist

# Output
* acb82a9 2025-05-06 | Oops, we didn't want this commit (HEAD -> master) [mgkl92]
* 034d418 2025-05-05 | Added a comment [mgkl92]
* 2c1e42d 2025-05-05 | Added a default value (tag: v1) [mgkl92]
* de82ecc 2025-05-05 | Unrelated change to c (tag: v1-beta) [mgkl92]
* 74cf2db 2025-05-05 | Changes for a and b [mgkl92]
* 55b8500 2025-05-05 | Using ARGV [mgkl92]
* 296786b 2025-05-05 | First Commit [mgkl92]

```

```shell
# 创建撤销提交
git revert HEAD

# 检查历史记录
* 446e8e4 2025-05-06 | Revert "Oops, we didn't want this commit" (HEAD -> master) [mgkl92]
* acb82a9 2025-05-06 | Oops, we didn't want this commit [mgkl92]
* 034d418 2025-05-05 | Added a comment [mgkl92]
* 2c1e42d 2025-05-05 | Added a default value (tag: v1) [mgkl92]
* de82ecc 2025-05-05 | Unrelated change to c (tag: v1-beta) [mgkl92]
* 74cf2db 2025-05-05 | Changes for a and b [mgkl92]
* 55b8500 2025-05-05 | Using ARGV [mgkl92]
* 296786b 2025-05-05 | First Commit [mgkl92]
```

现在，我们可以使用 git revert HEAD 命令撤销当前的提交记录，但 Git 并不会从提交历史中移除相应提交，而是引入了一个新的撤销提交。

# 17 移除分支中的提交

假设我们现在对哈希值 acb82a9 的提交记录添加了 oops 标签。

```shell
# 检查历史记录
git hist

# Output
* 446e8e4 2025-05-06 | Revert "Oops, we didn't want this commit" (HEAD -> master) [mgkl92]
* acb82a9 2025-05-06 | Oops, we didn't want this commit (tag: oops) [mgkl92]
* 034d418 2025-05-05 | Added a comment [mgkl92]
* 2c1e42d 2025-05-05 | Added a default value (tag: v1) [mgkl92]
* de82ecc 2025-05-05 | Unrelated change to c (tag: v1-beta) [mgkl92]
* 74cf2db 2025-05-05 | Changes for a and b [mgkl92]
* 55b8500 2025-05-05 | Using ARGV [mgkl92]
* 296786b 2025-05-05 | First Commit [mgkl92]
```

```shell
# 移除 master 分支中 v1 标签后面的提交记录
git reset --hard v1

# Output
HEAD is now at 2c1e42d Added a default value

# 检查历史记录
git hist

# Output
* 2c1e42d 2025-05-05 | Added a default value (HEAD -> master, tag: v1) [mgkl92]
* de82ecc 2025-05-05 | Unrelated change to c (tag: v1-beta) [mgkl92]
* 74cf2db 2025-05-05 | Changes for a and b [mgkl92]
* 55b8500 2025-05-05 | Using ARGV [mgkl92]
* 296786b 2025-05-05 | First Commit [mgkl92]
```

从历史记录中，我们可以看出 master 指针和 HEAD 指针指向了标签 v1 所在的提交记录，但实际仍可以引用到使用 oops 标签的提交记录。

```shell
# 检查所有历史记录
git hist --all

# Output 
* acb82a9 2025-05-06 | Oops, we didn't want this commit (tag: oops) [mgkl92]
* 034d418 2025-05-05 | Added a comment [mgkl92]
* 2c1e42d 2025-05-05 | Added a default value (HEAD -> master, tag: v1) [mgkl92]
* de82ecc 2025-05-05 | Unrelated change to c (tag: v1-beta) [mgkl92]
* 74cf2db 2025-05-05 | Changes for a and b [mgkl92]
* 55b8500 2025-05-05 | Using ARGV [mgkl92]
* 296786b 2025-05-05 | First Commit [mgkl92]
```

```shell
# 删除 oops 标签
git tag -d oops

# Output
Deleted tag 'oops' (was acb82a9)

# 检查所有历史记录

# Output 
* 2c1e42d 2025-05-05 | Added a default value (HEAD -> master, tag: v1) [mgkl92]
* de82ecc 2025-05-05 | Unrelated change to c (tag: v1-beta) [mgkl92]
* 74cf2db 2025-05-05 | Changes for a and b [mgkl92]
* 55b8500 2025-05-05 | Using ARGV [mgkl92]
* 296786b 2025-05-05 | First Commit [mgkl92]
```

但是，当我们使用 git tag -d 命令删除 oops 标签后，我们无法再通过任何指针获取该记录的引用。

# 19 修改提交

现在假设，我们对 hello.rb 进行了如下修改且已提交：

```txt
# Default is World
# Author: Jim Weirich
name = ARGV.first || "World"

puts "Hello, #{name}!"
```

```shell
# 检查历史记录
git hist

# Output
* 8135b6e 2025-05-06 | Add an author comment (HEAD -> master) [mgkl92]
* 2c1e42d 2025-05-05 | Added a default value (tag: v1) [mgkl92]
* de82ecc 2025-05-05 | Unrelated change to c (tag: v1-beta) [mgkl92]
* 74cf2db 2025-05-05 | Changes for a and b [mgkl92]
* 55b8500 2025-05-05 | Using ARGV [mgkl92]
* 296786b 2025-05-05 | First Commit [mgkl92]
```

此时，我们想要再添加一条邮箱的注释如下：

```txt
# Default is World
# Author: mgkl
# Email: zst_lff
name = ARGV.first || "World"

puts "Hello, #{name}!"
```

但是，我们并不想再产生一条新的提交记录。

```shell
# 将对 hello.rb 的变更添加到暂存区
git add hello.rb

# 修改先前的提交内容
git commit --amend -m "Add an author/email comment"

# 检查历史记录
git hist

# Output
* f80511d 2025-05-06 | Add an author/email comment (HEAD -> master) [mgkl92]
* 2c1e42d 2025-05-05 | Added a default value (tag: v1) [mgkl92]
* de82ecc 2025-05-05 | Unrelated change to c (tag: v1-beta) [mgkl92]
* 74cf2db 2025-05-05 | Changes for a and b [mgkl92]
* 55b8500 2025-05-05 | Using ARGV [mgkl92]
* 296786b 2025-05-05 | First Commit [mgkl92]
```

庆幸的是，使用 git commit --amend 命令能够让我们将本次提交与上次提交进行合并。

# 20 移动文件

```shell
mkdir lib

# 将 hello.rb 移动到 lib 目录中
git mv hello.rb lib

# 检查仓库状态
git st

# Output
On branch master
Changes to be committed:
  (use "git reset HEAD <file>..." to unstage)

        renamed:    hello.rb -> lib/hello.rb
```

```shell
# git mv 命令的等价操作
mv hello.rb lib
git add lib/hello.rb
git rm hello.rb

# 检查仓库状态
On branch master
Changes to be committed:
  (use "git reset HEAD <file>..." to unstage)

        renamed:    hello.rb -> lib/hello.rb

# 提交记录
git commit -m "Moved hello.rb to lib"

# Output
[master aecacc4] Moved hello.rb to lib
 1 file changed, 0 insertions(+), 0 deletions(-)
 rename hello.rb => lib/hello.rb (100%)
```

# 22 Git 内部原理之 .git 目录

```shell
# .git 的目录结构
ls -C .git

# Output
branches  COMMIT_EDITMSG  config  description  FETCH_HEAD  HEAD  hooks  index  info  logs  objects  ORIG_HEAD  refs
```

Git 将对象使用 sha1 算法计算的哈希值的前两位作为目录名称。

```shell
# Git 对象（Object）存储
ls -C .git/objects

# Output
03  0b  0d  11  20  25  27  28  29  2c  32  3d  43  44  48  55  59  6b  74  81  97  9c  a0  ac  ae  af  ba  ce  dc  de  e6  e7  f8  info  pack

# 深入探究 objects 子目录
ls -C .git/objects/03

# Output
4d418e709579f1b8142fef8dcb07ff577aea1f
```

Git 仓库中的 config 文件存储了当前仓库相关的配置。

```shell
# 查看配置文件内容
cat .git/config

# Output
[core]
        repositoryformatversion = 0
        filemode = true
        bare = false
        logallrefupdates = true
```

```shell
ls .git/refs 

# Output
heads  tags

ls .git/refs/heads

# Output
master

ls .git/refs/tags

# Ouput
v1  v1-beta

cat .git/refs/tags/v1

# Output
2c1e42dc3788479fb042d149fc97c9c1c5bf9ea
```

HEAD 文件用于追踪 HEAD 指针指向的记录。

默认，HEAD 指向当前分支的最新提交记录；但你可以使用 git checkout 命令来切换 HEAD 指向的提交记录。

```shell
cat .git/HEAD

# Output
ref: refs/heads/master

git checkout v1

cat .git/HEAD

# Output
2c1e42dc3788479fb042d149fc97c9c1c5bf9ea9
```

# 23 Git 内部原理之直接操纵 Git 对象

[原文连接](https://gitimmersion.com/lab_23.html)

- [ ] 待补充

# 24 创建分支

```shell
# 创建并切换到 greet 分支
git checkout -b greet

# Output
Switched to a new branch 'greet'

# 检查仓库状态
git st

# Output
On branch greet
nothing to commit, working tree clean
```

现在假设, 我们在 greet 分支创建了如下 lib/greeter.rb 文件并已提交:

```txt
class Greeter
  def initialize(who)
    @who = who
  end
  def greet
    "Hello, #{@who}"
  end
end
```

现在, 我们对 lib/hello.rb 文件作如下修改并提交:

```txt
require 'greeter'

# Default is World
# Author: mgkl
# Email: zst_lff
name = ARGV.first || "World"

greeter = Greeter.new(name)
puts greeter.greet
```

现在, 我们对 Rakefile 文件作如下修改并提交:

```txt
#!/usr/bin/ruby -wKU

task :default => :run

task :run do
  ruby '-Ilib', 'lib/hello.rb'
end
```

至此，我们已经在 greet 分支上进行了三次提交。

现在，我们在 master 分支上创建了如下 README 文件并已提交。

```shell
This is the Hello World example from the git tutorial.
```

## 25 切换分支

使用 git branch 命令允许我们进行分支切换。

```shell
# 切换到 master 分支
git checkout master

# Output
Switched to branch 'master'

# 切换到 greet 分支
git checkout greet
# Output
Switched to branch 'greet'

# 切回分支
git checkout -

# Output
Switched to branch 'master'
```

# 28 合并分支

```shell
# 检视所有分支上的提交记录
git hist --all

# Output
* 8a9c07e 2025-05-06 | Added README (HEAD -> master) [mgkl92]
| * 9025f0a 2025-05-06 | Updated Rakefile (greet) [mgkl92]
| * 366bd33 2025-05-06 | Hello uses Greeter [mgkl92]
| * 35f07c2 2025-05-06 | Added greeter class [mgkl92]
|/  
* 0d15d6c 2025-05-06 | Added a Rakefile [mgkl92]
* aecacc4 2025-05-06 | Moved hello.rb to lib [mgkl92]
* f80511d 2025-05-06 | Add an author/email comment [mgkl92]
* 2c1e42d 2025-05-05 | Added a default value (tag: v1) [mgkl92]
* de82ecc 2025-05-05 | Unrelated change to c (tag: v1-beta) [mgkl92]
* 74cf2db 2025-05-05 | Changes for a and b [mgkl92]
* 55b8500 2025-05-05 | Using ARGV [mgkl92]
* 296786b 2025-05-05 | First Commit [mgkl92]
```

现在，让我们切换到 greet 分支并将 master 分支合并到 greet。

```shell
# 合并 master 分支
git merge master

# Output
Merge made by the 'recursive' strategy.
 README | 1 +
 1 file changed, 1 insertion(+)
 create mode 100644 README

# 查看历史记录
git hist

# Output
*   d52de78 2025-05-06 | Merge branch 'master' into greet (HEAD -> greet) [mgkl92]
|\  
| * 8a9c07e 2025-05-06 | Added README (master) [mgkl92]
* | 9025f0a 2025-05-06 | Updated Rakefile [mgkl92]
* | 366bd33 2025-05-06 | Hello uses Greeter [mgkl92]
* | 35f07c2 2025-05-06 | Added greeter class [mgkl92]
|/  
* 0d15d6c 2025-05-06 | Added a Rakefile [mgkl92]
* aecacc4 2025-05-06 | Moved hello.rb to lib [mgkl92]
* f80511d 2025-05-06 | Add an author/email comment [mgkl92]
* 2c1e42d 2025-05-05 | Added a default value (tag: v1) [mgkl92]
* de82ecc 2025-05-05 | Unrelated change to c (tag: v1-beta) [mgkl92]
* 74cf2db 2025-05-05 | Changes for a and b [mgkl92]
* 55b8500 2025-05-05 | Using ARGV [mgkl92]
* 296786b 2025-05-05 | First Commit [mgkl92]
```

# 29 创建冲突

现在，我们在 master 分支上修改 lib/hello.rb 的内容如下并提交：

```txt
# Default is World
# Author: mgkl
# Email: zst_lff
puts "What's your name?"
my_name = gets.strip

puts "Hello, #{my_name}!"
```

```shell
# 查看所有分支上的提交记录
git hist --all

# Output
* b992e23 2025-05-06 | Made interactive (HEAD -> master) [mgkl92]
| *   d52de78 2025-05-06 | Merge branch 'master' into greet (greet) [mgkl92]
| |\  
| |/  
|/|   
* | 8a9c07e 2025-05-06 | Added README [mgkl92]
| * 9025f0a 2025-05-06 | Updated Rakefile [mgkl92]
| * 366bd33 2025-05-06 | Hello uses Greeter [mgkl92]
| * 35f07c2 2025-05-06 | Added greeter class [mgkl92]
|/  
* 0d15d6c 2025-05-06 | Added a Rakefile [mgkl92]
* aecacc4 2025-05-06 | Moved hello.rb to lib [mgkl92]
* f80511d 2025-05-06 | Add an author/email comment [mgkl92]
* 2c1e42d 2025-05-05 | Added a default value (tag: v1) [mgkl92]
* de82ecc 2025-05-05 | Unrelated change to c (tag: v1-beta) [mgkl92]
* 74cf2db 2025-05-05 | Changes for a and b [mgkl92]
* 55b8500 2025-05-05 | Using ARGV [mgkl92]
* 296786b 2025-05-05 | First Commit [mgkl92]
```

# 30 解决冲突

```shell
# 尝试将 master 分支合并到 greet 分支
git merge master
```

此时，因为我们同时在 master 分支和 branch 分支的 lib/hello.rb 中的同一位置进行了修改；所以，上述合并将需要我们手动解决两个文件中的冲突内容，大致如下：

```txt
<<<<<<< HEAD
require 'greeter'

# Default is World
name = ARGV.first || "World"

greeter = Greeter.new(name)
puts greeter.greet
=======
# Default is World

puts "What's your name"
my_name = gets.strip

puts "Hello, #{my_name}!"
>>>>>>> main
```

```shell
# 将手动解决冲突后的文件添加到暂存区
git add lib/hello.rb

# 提交解决冲突后的合并记录
git commit -m "Merged master fixed conflict"
```

# 31 变基（Rebasing）与合并（Merging）

[原文连接](https://gitimmersion.com/lab_31.html)

- [ ] 待补充

# 36 多仓库

```shell
# 克隆本地仓库 hello 为 cloned_hello
git clone hello cloned_hello

# Output
Cloning into 'cloned_hello'...
done.

# 检查 cloned_hello 的提交历史
git hist --all

# Output
*   f2e2ed1 2025-05-06 | Merged main fixed conflict. (HEAD -> greet, origin/greet, origin/HEAD) [mgkl92]
|\  
| * eee1cf6 2025-05-06 | Made interactive (origin/master) [mgkl92]
* |   d52de78 2025-05-06 | Merge branch 'master' into greet [mgkl92]
|\ \  
| |/  
| * 8a9c07e 2025-05-06 | Added README [mgkl92]
* | 9025f0a 2025-05-06 | Updated Rakefile [mgkl92]
* | 366bd33 2025-05-06 | Hello uses Greeter [mgkl92]
* | 35f07c2 2025-05-06 | Added greeter class [mgkl92]
|/  
* 0d15d6c 2025-05-06 | Added a Rakefile [mgkl92]
* aecacc4 2025-05-06 | Moved hello.rb to lib [mgkl92]
* f80511d 2025-05-06 | Add an author/email comment [mgkl92]
* 2c1e42d 2025-05-05 | Added a default value (tag: v1) [mgkl92]
* de82ecc 2025-05-05 | Unrelated change to c (tag: v1-beta) [mgkl92]
* 74cf2db 2025-05-05 | Changes for a and b [mgkl92]
* 55b8500 2025-05-05 | Using ARGV [mgkl92]
* 296786b 2025-05-05 | First Commit [mgkl92]

# 查看 cloned_hello 的所有分支
git branch --all

# Output
* greet
  remotes/origin/HEAD -> origin/greet
  remotes/origin/greet
  remotes/origin/master
```

# 39 Origin 仓库

```shell
# 查看当前仓库关联的远程仓库
git remote

# Output
origin

# 查看 origin 的相关信息
git remote show origin

# Output
* remote origin
  Fetch URL: /home/mgkl/Desktop/git_tutorial/hello
  Push  URL: /home/mgkl/Desktop/git_tutorial/hello
  HEAD branch: greet
  Remote branches:
    greet  tracked
    master tracked
  Local branch configured for 'git pull':
    greet merges with remote greet
  Local ref configured for 'git push':
    greet pushes to greet (up to date)
```

# 40 远程分支

默认情况下，git branch 命令仅列出当本地仓库的分支。

```shell
# 查看分支
git branch

# Output
* greet
```

然而，git branch [-a | --all] 命令会将本地仓库中用于追踪远程仓库的追踪分支也列举出来。

```shell
# 查看所有分支
git branch --all

# Output
* greet
  remotes/origin/HEAD -> origin/greet
  remotes/origin/greet
  remotes/origin/master
```

值得注意的是，追踪分支由 Git 系统自动管理。

# 42 拉取变更

现在假设，我们在 hello 仓库对 README 文件作如下修改并提交：

```txt
This is the Hello World example from the git tutorial.
(changed in original)
```

```shell
git add README
git commit -m "Changed README in original repo"
```

使用 git fetch 可以从远程仓库 cloned_hello 拉取上述变更；但默认，git fethc 命令并不会自动为我们进行分支的合并操作。

```shell
git fetch

git hist --all

# Output
* f9db1c2 2025-05-06 | Changed README in original repo (origin/greet, origin/HEAD) [mgkl92]
*   f2e2ed1 2025-05-06 | Merged main fixed conflict. (HEAD -> greet) [mgkl92]
|\  
| * eee1cf6 2025-05-06 | Made interactive (origin/master) [mgkl92]
* |   d52de78 2025-05-06 | Merge branch 'master' into greet [mgkl92]
|\ \  
| |/  
| * 8a9c07e 2025-05-06 | Added README [mgkl92]
* | 9025f0a 2025-05-06 | Updated Rakefile [mgkl92]
* | 366bd33 2025-05-06 | Hello uses Greeter [mgkl92]
* | 35f07c2 2025-05-06 | Added greeter class [mgkl92]
|/  
* 0d15d6c 2025-05-06 | Added a Rakefile [mgkl92]
* aecacc4 2025-05-06 | Moved hello.rb to lib [mgkl92]
* f80511d 2025-05-06 | Add an author/email comment [mgkl92]
* 2c1e42d 2025-05-05 | Added a default value (tag: v1) [mgkl92]
* de82ecc 2025-05-05 | Unrelated change to c (tag: v1-beta) [mgkl92]
* 74cf2db 2025-05-05 | Changes for a and b [mgkl92]
* 55b8500 2025-05-05 | Using ARGV [mgkl92]
* 296786b 2025-05-05 | First Commit [mgkl92]

# 合并远程分支 origin/greet 到 greet 分支
git merge origin/greet

# Output
Updating f2e2ed1..f9db1c2
Fast-forward
 README | 3 ++-
 1 file changed, 2 insertions(+), 1 deletion(-)
```

Git 的 git pull 命令将自动为我们拉取远程分支的变更并进行合并，它等价于先使用 git fetch 命令再使用 git merge 命令（Git 会默认合并到当前分支）。

# 45 在本地添加追踪分支

```shell
# 使用本地 master 分支追踪 origin/master 分支（远程仓库 hello 的 master 分支）
git branch --track master origin/master

# Output
Branch 'master' set up to track remote branch 'master' from 'origin'.


# 检查所有分支
git branch --all

# Output
* greet
  master
  remotes/origin/HEAD -> origin/greet
  remotes/origin/greet
  remotes/origin/master
```

# 46 创建裸仓库

裸仓库不包含工作目录，仅包含 .git 目录。

```shell
# 创建 hello 仓库的裸仓库 hello.git
git clone --bare hello hello.git

# Output
Cloning into bare repository 'hello.git'...
done.

# 查看 hello.git 仓库目录结构
ls hello.git

# Output
branches  config  description  HEAD  hooks  info  objects  packed-refs  refs


# 将 hello.git 添加为 hello 仓库的远程仓库并命名为 shared
git remote add  shared ../hello.git/

# 查看远程仓库
git remote show shared

# Output
* remote shared
  Fetch URL: ../hello.git/
  Push  URL: ../hello.git/
  HEAD branch: greet
  Remote branches:
    greet  new (next fetch will store in remotes/shared)
    master new (next fetch will store in remotes/shared)
  Local refs configured for 'git push':
    greet  pushes to greet  (up to date)
```

# 49 推送变更

现在假设，我们对 hello 仓库的 README 文件作如下修改并提交：

```txt
This is the Hello World example from the git tutorial.
(Changed in the original and pushed to shared)
```

```shell
git add README
git commit -m "Added shared comment to readme"
```

```shell
# 将对 README 的变更推送到远程仓库 shared
git push shared greet

# Output
Enumerating objects: 5, done.
Counting objects: 100% (5/5), done.
Delta compression using up to 4 threads
Compressing objects: 100% (3/3), done.
Writing objects: 100% (3/3), 362 bytes | 362.00 KiB/s, done.
Total 3 (delta 1), reused 0 (delta 0)
To ../hello.git/
   f9db1c2..33dfb3a  greet -> greet
```

# 50 托管你的 Git 仓库

[原文连接](https://gitimmersion.com/lab_50.html)

- [ ] 待补充