#include <vector>
#include <algorithm>
#include <queue>
#include <climits>

using std::vector;
using std::reverse;
using std::queue;
using std::priority_queue;

// 课程表(图结构的环检测 - DFS 版)
vector<bool> onPath;   // 记录一次 DFS 过程中经过的点
vector<bool> visited;  // 记录节点是否被遍历
bool hasCycle = false; // 记录图 G 是否存在环

void traverse(vector<vector<int>> &G, int i) {
    if (hasCycle) {
        return;
    }

    if (onPath[i]) {
        hasCycle = true;
        return;
    }

    if (visited[i]) {
        return;
    }

    visited[i] = true;

    onPath[i] = true;
    for (int &j : G[i]) {
        traverse(G, j);
    }
    onPath[i] = false;
}

bool canFinish(int n, vector<vector<int>> &prequisites) {
    // 从依赖关系中构建邻接矩阵
    vector<vector<int>> G = vector<vector<int>>(n, vector<int>());

    onPath = vector<bool>(n);
    visited = vector<bool>(n, false);

    for (const auto &prequisite : prequisites) {
        int from = prequisite[1], to = prequisite[0];
        G[from].push_back(to);
    }

    for (size_t i = 0; i < n; ++i) {
        traverse(G, i);
    }

    return !hasCycle;
}

// 合理课程表安排

// 后序遍历结果
vector<int> postorder;
bool hasCycle = false;
vector<bool> visited, onPath;

void traverse(vector<vector<int>> &G, int i) {
    if (onPath[i]) {
        hasCycle = true;
    }

    if (visited[i] || hasCycle) {
        return;
    }

    visited[i] = true;
    onPath[i] = true;

    for (int &j : G[i]) {
        traverse(G, j);
    }
    postorder.push_back(i); // 记录后续遍历位置
    onPath[i] = false;
}

vector<int> findOrder(int n, vector<vector<int>> &prequisites) {
    onPath = vector<bool>(n);
    visited = vector<bool>(n, false);

    vector<vector<int>> G = vector<vector<int>>(n, vector<int>());
    for (const auto &prequisite : prequisites) {
        int from = prequisite[1], to = prequisite[0];
        G[from].push_back(to);
    }

    for (size_t i = 0; i < n; ++i) {
        traverse(G, i);
    }

    if (hasCycle) {
        return {};
    }

    reverse(postorder.begin(), postorder.end());

    return postorder;
}

// 课程表(图结构的环检测 - BFS 版)
bool canFinish(int n, vector<vector<int>> &prequisites) {
    vector<vector<int>> G = vector<vector<int>>(n, vector<int>());
    for (const auto &prequisite : prequisites) {
        int from = prequisite[1], to = prequisite[0];
        G[from].push_back(to);
    }

    vector<int> indeg(n);
    for (auto &prequisite : prequisites) {
        int to = prequisite[0];
        ++indeg[to];
    }

    queue<int> q;
    for (size_t i = 0; i < n; ++i) {
        if (indeg[i] == 0) { // 没有节点依赖于节点 i
            q.push(i);
        }
    }

    int cnt = 0;
    while (!q.empty()) {
        int i = q.front();
        q.pop();

        ++cnt;
        for (int j : G[i]) {
            if (--indeg[j]) {
                q.push(j);
            }
        }
    }

    return cnt == n;
}

// 二分图判断
bool ok = true;
vector<bool> color;
vector<bool> visited;

void traverse(const vector<vector<int>> &G, int i) {
    if (!ok) { return; }

    visited[i] = true;
    for (const int &j : G[i]) {
        if (!visited[j]) { // 节点 j 尚未被访问
            color[j] = !color[i];
        } else {
            if (color[i] == color[j]) {
                ok = false;
                return;
            }
        }
    }
}

bool isBipartite(vector<vector<int>> &G) {
    int n = G.size();
    color = vector<bool>(n, false);
    visited = vector<bool>(n, false);

    for (size_t i = 0; i < n; ++i) {
        if (!visited[i]) {
            traverse(G, i);
        }
    }
}

// 单源最短路径算法 Djkstra 算法
// Dijkstra 算法是一种贪心算法，需要保证图的边权值为正；
// 否则，贪心思想失效（经过的边越多权重越大）。
int n;                                    // 节点数目
vector<vector<int>> G(n, vector<int>(n)); // 图的邻接矩阵
vector<int> dist(n);                      // src 记录 src 到其他各点的距离
vector<bool> visited(n);                  // 记录节点是否被访问过

void dijkstra(int src) {
    // 初始化访问状态
    for (size_t i = 0; i < n; ++i) {
        dist[i] = G[src][i];
        visited[i] = false;
    }

    for (size_t i = 0; i < n; ++i) {
        int min = INT_MAX, u = -1;

        for (int j = 0; j < n; ++j) {
            if (dist[j] < min && !visited[j]) {
                u = j;
                min = dist[j];
            }
        }

        if (u == -1) {
            return;
        }

        visited[u] = true;
        for (size_t v = 0; v < n; ++v) {
            if (!visited[v] && G[u][v] != INT_MAX && dist[v] > dist[u] + G[u][v]) {
                dist[v] = dist[u] + G[u][v];
            }
        }
    }
}

// Prim 最小生成树算法
// Prim 算法将顶点根据是否加入最小生成树分为两个集合 A 和 B
// 初始化任意一个顶点加入到 A
// 从 B 集合顶点中选择一个到 A 集合定点中距离最小的点加入到集合 A
// 重复上述过程
struct State {
    int u;
    int v;
    int w;
    State(int u, int v, int w) :
        u(u), v(v), w(w) {
    }
};

vector<vector<int>> Prim(vector<vector<int>> G, int src) {
    vector<vector<int>> res(n, vector<int>(n, INT_MAX));
    int n = G.size();

    auto cmp = [](const State &lhs, const State &rhs) {
        return lhs.w < rhs.w;
    };
    priority_queue<State, vector<State>, decltype(cmp)> pq(cmp);

    visited[src] = true;
    for (size_t i = 0; i < n; ++i) {
        if (!visited[i] && G[src][i] != INT_MAX) {
            pq.emplace(src, i, G[src][i]);
        }
    }

    int cnt = n;
    while (--n) {
        State s = pq.top();
        int u = s.u, v = s.v, w = s.w;

        G[u][v] = s.w;
        visited[v] = true;
        for (size_t i = 0; i < n; ++i) {
            if (!visited[i] && G[v][i] != INT_MAX) {
                pq.emplace(v, i, G[src][i]);
            }
        }
    }

    return res;
}

// Kruskal 算法
// Kruskal 算法将边的权值按升序排列
// 每次选择一条边插入到最小生成树的集合中
// 连通性通过并查集来判断