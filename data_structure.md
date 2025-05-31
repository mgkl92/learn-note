# 并查集

并查集用于管理元素的集合所属问题。

一个集合由一棵树构成，属于相同集合的元素位于同一棵树上，它们有相同的根结点；而属于不同集合的元素位于不同的树上。

所以，维护元素的集合所属问题在本质上是维护一个森林。

```cpp
// 初始化操作
struct dsu {
    vector<size_t> pa;

    explicit dsu(size_t size) : pa(size) {
        // 初始时，每个元素都位于单独的集合
        // 为了区别不同的根，将所有结点的根初始为自己
        std::iota(pa.begin(), pa.end(), 0);
    }

    size_t find(size_t x);

    void unite(size_t x, size_t);

    void erase(size_t x);

    void move(size_t x, size_t y);
};

// 查询操作
size_t dsu::find(size_t x) {
    // 递归的查询父结点的根结点直至到达当根节点
    return pa[x] == x ? x : find(pa[x]);
}

// 路径压缩
size_t dsu::find(size_t x) {
    // 将结点直接连接到根节点
    return pa[x] == x ? x : pa[x] = find(pa[x]);
}

// 合并两个集合
void dsu::unite(size_t x, size_t y) {
    pa[find(x)] = find(y);
}

// 合并优化
struct dsu {
    // 使用 size 记录每颗树上结点数目
    vector<size_t> pa, size;
    
    ...

    void unite(size_t x, size_t y);
};

void dsu::unite(size_t x, size_t y) {
    x = find(x);
    y = find(y);

    if (x == y) { return ;}
    if (size[x] < size[y]) {
        swap(x, y);
    }
    pa[y] = x;
    size[x] += size[y];
}

// 删除操作

struct dsu {
    vector<size_t> pa, size;

    // 为每个结点预先指定副本
    // 初始化将每个结点指向其副本，不再指向其自身
    explicit dsu(size_t size_) : pa(size_ * 2), size(size_ * 2, 1) {
        std::iota(pa.begin(), pa.begin() + size_, size_);
        std::iota(pa.begin() + size_, pa.end(), size_);
    }
    
    void erase(size_t x) {
        --size[find(x)];
        
        // 让删除的元素指向自身
        pa[x] = x;
    }
}

// 移动操作
void dsu::move(size_t x, size_t y) {
    auto px = find(x), py = find(y);

    if (px == py) { return ; }

    // 将 x 所在集合移动到 y 所在集合
    pa[x] = fy;
    --size[px]. ++size[py];
}
```

# 堆

堆在本质上是一颗树，树中的每个结点存储一个键值，且每个结点的键值都大于等于或小于等于其父结点的键值。

## 二叉堆

二叉堆是一颗完全二叉树。

```cpp
// 索引从 1 开始，当根结点为 j 时，其父结点为 j / 2 向下取整， 例如 2 和 3 的父节点为 1。
// h 代表大顶堆数组

// 插入操作
// 向上调整：在堆数组末尾插入元素，动态的向上调整
void up(int x) {
    while (x > 1 && h[x] > h [x / 2]) {
        std::swap(h[x], h[x / 2]);
        x /= 2;
    }
}

// 删除操作
// 向下调整：交换堆顶与堆数组末尾元素，动态的向下调整，并删除堆数组末尾元素
void down(int x) {
    while (x * 2 <= n) {
        // 令 t 指向 x 具有最大值的子节点
        int t = x * 2;
        if（t + 1 <= n && h[t + 1] > h[t]) {
            ++t;
        }
        
        // 判断是否满足堆性质
        if (h[t] <= h[x]) { break; }

        std::swap(h[x], h[t]);

        // 迭代的调整子堆
        x = t;
    }
}

// 堆初始化
// 不断地向堆中添加元素
void build(int n) {
    for (int i = 1; i <= n; ++i) {
        up(i);
    }
}

// 自定向上不断地合并两个堆
void build(int n) {
    for (int i = n; n >= 1; --i) {
        down(i);
    }
}
```

**应用：对顶堆**

动态的维护一个序列上第 k 大（小）的值， k 值可能动态变化。

对顶堆由一个大根堆和一个小根堆组成，其中

# 单调栈

单调栈是满足单调性质的栈结构。

```cpp
std::stack<int> st;
int x;
...
// 在插入元素时，为了维护栈中元素的单调性质，需要弹出栈内元素值小于待插入元素的部分。
while (!st.empty() && st.top() < x) {
    st.pop();
}
st.push(x);
```

# 单调队列

单调队列时满足单调性质的队列结构，类似单调栈的思路。

# 树状数组

树状数组是一种支持单点修改和区间查询的数据结构。

```cpp

```

# 线段树

# 二叉搜索树 BST

**基本定义**

- 空树是二叉搜索树；

- 如果左子树不为空，则左子树上所有结点的权值小于其根结点；

- 如果右子树不为空，则右子树上所有结点的权值大于其根节点；
  
- 二叉搜索树的左右子树均为二叉搜索树。

**代码实现**

```cpp
// 结点定义
struct TreeNode {
    int key; // 权值
    TreeNode *left, *right;
    int size; // 以当前结点为根的子树中的结点数目
    int count; // 权值重复数量
};

// BST 的中序遍历的权值序列具有单调不减的性质！
void inOrderTraverse(TreeNode *root) {
    if (root == nullptr) {
        return;
    }

    inOrderTraverse(root->left);
    std::cout << root->key << " ";
    inOrderTraverse(root->right);
}

// 最值查询
// 根据平衡二叉树的性质，其最小值和最大值分别存储于最左侧叶子和最右侧叶子结点上
int min(TreeNode *root) {
    if (root == nullptr) {
        return -1;
    }

    while (root->left != nullptr) {
        root = root->left;
    }

    return root->key;
}

int max(TreeNode *root) {
    if (root == nullptr) {
        return -1;
    }

    while (root->right != nullptr) {
        root = root->right;
    }

    return root->key;
}

// 元素查询
// 二叉搜索树允许我们以二分形式对树中的元素数据进行查询
bool search(TreeNode *root, int target) {
    if (root == nullptr) {
        return false;
    }

    if (root->key == target) {
        return true;
    } else if (root->key > target) {
        // 查找左子树
        return search(root->left, target);
    } else {
        // 查找右子树
        return search(root->right, target);
    }
}

// 元素插入
// 通过二分查找方法确定元素的插入位置
TreeNode* insert(TreeNode* root, int value) {
    if (root == nullptr) {
        return new TreeNode(value);
    }

    if (root->key == value) {
        ++root->count;
    } else if (root->key > value) {
        // 在左子树中插入
        root->left = insert(root->left, value);
    } else {
        // 在右子树中插入
        root->right = insert(root->right, value);
    }

    return root;
}

// 元素删除
TreeNode* erase(TreeNode *root, int value) {
    if (root == nullptr) {
        return root;
    }

    if (root->key == value) {
        if (root->count > 1) {
            --root->count;
        } else {
            if (root->left == nullptr && root->left) {
                // 左节点为空
                TreeNode *p = root->right;
                delete root;
                return p;
            } else if (root->right == nullptr) {
                // 右节点为空
                TreeNode *p = root->left;
                delete root;
                return p;
            } else {
                // 非叶子结点
                // 使用右子树的最小代值结点替当前结点（此处也可以使用左子树的最大值结点）
                TreeNode * q = min(root->right);
                root->key = q->key;
                root->count = q->count;
                root->right = erase(root->right, q->key);
            }
        }
    } else if (root->key > value) {
        root->left = erase(root->left, value);
    } else {
        root->right = erase(root->right, value);
    } 

    // 更新结点数目信息
    root->size = root->count 
        + (root->left ? root->left->size : 0)
        + (root->right ? root->right->size : 0);

    return root;
}

// 元素排名
// 排名定义为将数组元素升序排序后第一个相同元素之前的数的个数加一
int getRank(TreeNode *root, int value) {
    if (root == nullptr) {
        return 0;
    }

    if (root->key == value) {
        return (root->left ? root->left->size : 0) + 1;
    }

    if (root->key > v) {
        return getRank(root->left, value);
    }

    return getRank(root->right, value) + (root->left ? root->left->size : 0) + root->count;
}

int rank(TreeNode *root, int k) {
    if (root == nullptr) {
        return -1;
    }

    if (root-left) {
        // 在左子树中查询排名为 k 的元素
        if (root->left->size >= k) {
            return rank(root->left, k);
        }

        // 当前结点为为排名为 k 
        if ((root->left->size + root->count) >= k) {
            return root->key;
        }
    } else {
        // 避免 k 计算为 0
        if (k == 1) {
                return root->key;
        }
    }

    return rank(root->right, k - (root->left ? root->left->size : 0) - root->count);
}
```

## 平衡二叉树

在理想情况下，二叉搜索树的具有 O(log(n)) 的时间复杂度；但在最坏情况下，搜索树可能退化为链表，使得其时间复杂度为 O(n)。因此，为了使得二叉树的平均时间复杂度 O(log(n)) 级别，需要在结点插入和删除过程中动态的调正树的结构。

**二叉树调整操作**

```cpp
TreeNode* rorate_right(TreeNode* root) {
    TreeNode* temp = root->left;
    root->left = temp->right;
    temp->left = root;
    
    // 更新树高度
    update_height(root);
    update_height(temp);

    return temp;
}

TreeNode* rotate_left(TreeNode* root) {
    TreeNode *temp = root->right;
    root->right = temp->right;
    temp->right = root;

    update_height(root);
    update_height(tmep);

    return temp;
}
```

**四种破坏二叉树平衡的条件**

- LL 型

    ![bst-ll](pics/bst-ll.svg)

    形成原因：T 的左子树的左子树过高

    调整方式：右旋结点 T

- RR 型

    ![bst-rr](pics/bst-rr.svg)

    形成原因：T 的右子树的右子树过高
 
    调整方式：左旋结点 T

- LR 型

    ![bst-lr](pics/bst-lr.svg)

    形成原因：T 的左子树的右子树过高

    调整方式：先左旋结点 L，使其成为 LL 型；然后，再右旋结点 T

- RL 型

    ![bst-lr](pics/bst-lr.svg)

    形成原因：T 的右子树的左子树过高

    调整方式：先右旋结点 R，使其成为 RR 型；然后，再左旋结点 T


## AVL 树

AVL 树是一种平衡的二叉搜索树。

**基本定义**

- 空二叉树是一棵 AVL 树

- 如果 AVL 存在左右子树，那么其左右子树也是 AVL 树，且左右子树的高度差的决定是小于等于 1

- AVL 树的高度为 O(log(n))

## B 树

**基本定义**

- 每个结点至多由 m 个子节点；

- 每一个非叶子结点至少有 (m + 1) / 2 个子结点，即 m / 2 上取整；

- 如果根结点不是叶子结点，它至少有两个子节点；

- 有 k 个子结点的非叶子结点有 k - 1 元素，且元素之间升序排列，即满足 k[i] < k[i+1]；

- 所有的叶子结点位于同一层。

![b-tree](pics/b-tree-example.svg)

```cpp
// 查找键值为 k 的树节点
BTreeNode* search(int k) {
    int i = 0;
    while (i < n && k > keys[i]) {
        ++i;
    }

    if (keys[i] == k) {
        return this;
    }
    
    if (leaf == true) {
        return nullptr;
    }

    // 递归查询
    return C[i]->search(k);
}
```

## B+ 树

## 红黑树

**性质**

- 节点为红色或黑色

- NIL 节点（空叶子节点）为黑色
  
- 红色节点的子节点为黑色

- 从根节点到 NIL 节点每条路径上黑色节点数目相同