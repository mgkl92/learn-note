#include <vector>
#include <algorithm>

using std::vector;
using std::sort;
using std::max;

// 汽车加油问题
int canCompleteCircuit(vector<int> &gas, vector<int> &cost) {
    int n = gas.size();

    int sum = 0, minSum = 0, start = 0;

    for (int i = 0; i < n; ++i) {
        // 计算汽车经过第 i 个站点的变化
        sum += gas[i] - cost[i];

        // 选择图像中最低点作为起始点
        if (sum < minSum) {
            start = i + 1;
            minSum = sum;
        }
    }

    // 判断整体油耗是否满足条件
    if (sum < 0) {
        return -1;
    }

    return start % n;
}

// 计算最多无重叠子区间数目
int intervalSchedule(vector<vector<int>> &intvs) {
    if (intvs.empty()) {
        return 0;
    }

    // 按照结束事件非递减排序
    sort(intvs.begin(), intvs.end(), [](vector<int> &a, vector<int> &b) {
        return a[1] < b[1];
    });

    int count = 1;
    int x_end = intvs[0][1];
    for (const auto &interval : intvs) {
        int start = interval[0];
        if (start >= x_end) {
            ++count;
            x_end = interval[1];
        }
    }

    return count;
}

// 会议室安排
int minMeetingRooms(vector<vector<int>> &meetings) {
    int n = meetings.size();
    vector<int> start_times(n), end_times(n);
    for (size_t i = 0; i < n; ++i) {
        start_times[i] = meetings[i][0];
        end_times[i] = meetings[i][1];
    }

    sort(start_times.begin(), start_times.end());
    sort(end_times.begin(), end_times.end());

    int cnt = 0;
    int res = 0, i = 0, j = 0;
    while (i < n && j < n) {
        if (start_times[i] < end_times[j]) {
            // 扫描到一个会议开始
            ++cnt;
            ++i;
        } else {
            // 扫描到一个会议结束
            --cnt;
            ++j;
        }
        res = max(res, cnt);
    }
    return res;
}