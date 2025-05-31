#include <vector>
#include <list>
#include <algorithm>

using std::vector;
using std::list;
using std::sort;

// 全排列
vector<vector<int>> res;
list<int> track;
vector<bool> used;

// nums 无重复
void backtrack(vector<int> &nums) {
    if (track.size() == nums.size()) {
        res.push_back((vector<int>(track.begin(), track.end())));
        return;
    }

    for (int i = 0; i < nums.size(); ++i) {
        if (used[i]) {
            continue;
        }

        // 选择
        used[i] = true;
        track.push_back(nums[i]);

        // 递归
        backtrack(nums);

        // 撤销（回溯）
        used[i] = false;
        track.pop_back();
    }
}

vector<vector<int>> permutate(vector<int> &nums) {
    used = vector<bool>(nums.size(), false);

    backtrack(nums);

    return res;
}

// 子集/组合（元素无重）
// vector<vector<int>> res;
// list<int> track;

// void backtrack(vector<int> &nums, int start) {
//     res.push_back(vector<int>(track.begin(), track.end()));

//     for (int i = start; i < nums.size(); ++i) {
//         // 跳过重复元素
//         if (i > start && nums[i] == nums[i - 1]) {
//             continue;
//         }
        
//         // 选择
//         track.push_back(nums[i]);

//         // 递归
//         backtrack(nums, i + 1);

//         // 撤销（回溯）
//         track.pop_back();
//     }
// }

// vector<vector<int>> subsetWithDup(vector<int> &nums) {
//     sort(nums.begin(), nums.end());

//     backtrack(nums, 0);

//     return res;
// }

vector<vector<int>> res;
list<int> track;
int trackSum = 0;
void backtrack(vector<int> &nums, int start) {
    res.push_back(vector<int>(track.begin(), track.end()));

    for (int i = start; i < nums.size(); ++i) {
        // 跳过重复元素
        if (i > start && nums[i] == nums[i - 1]) {
            continue;
        }
        
        // 选择
        track.push_back(nums[i]);

        // 递归
        backtrack(nums, i + 1);

        // 撤销（回溯）
        track.pop_back();
    }
}

vector<vector<int>> subsetWithDup(vector<int> &nums) {
    sort(nums.begin(), nums.end());

    backtrack(nums, 0);

    return res;
}