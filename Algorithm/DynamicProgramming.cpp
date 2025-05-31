#include <string>
#include <vector>

using std::string;
using std::vector;

int substr(string &str1, string &str2) {
    int m = str1.length();
    int n = str2.length();

    vector<vector<int>> dp(m, vector<int>(n));
    for (int i = 0; i < m; ++i) {
        dp[i][0] = (str1[i] == str2[0]) ? 1 : 0;
    }

    for (int j = 0; j < n; ++j) {
        dp[0][j] = (str1[0] == str2[j]) ? 1 : 0;
    }

    for (int i = 1; i < m; ++i) {
        for (int j = 1; j < n; ++i) {
            if (str1[i] == str2[j]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            }
        }
    }

    int longest = 0;
    int longest_index = 0;

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (longest < dp[i][j]) {
                longest_index = i;
            }
        }
    }

    return longest;
}

int LongestPalindromicSubString(string &a) {
    int n = a.length();

    vector<vector<int>> dp(n, vector<int>(n, 0));
    for (size_t i = 0; i < n; ++i) {
        dp[i][i] = 1;
    }

    int max_len = 1;
    for (size_t i = n - 2; i >= 0; --i) {
        for (int j = i + 1; j < n; ++j) {
            if (a[i] == a[j]) {
                if (j - i == 1) {
                    dp[i][j] = 2;
                } else {
                    if (j - i > 1) {
                        dp[i][j] = dp[i + 1][j - 1] + 2;
                    }
                }

                if (max_len < dp[i][j]) {
                    max_len = dp[i][j];
                }
            } else {
                dp[i][j] = 0;
            }
        }
    }

    return max_len;
}

// vector<vector<int>> memo;

// int dp(const string &s1, int i, const string &s2, int j) {
//     if (i == s1.length() || j == s2.length()) {
//         return 0;
//     }

//     if (memo[i][j] != -1) {
//         return memo[i][j];
//     }

//     if (s1[i] == s2[j]) {
//         memo[i][j] = dp(s1, i + 1, s2, j + 1) + 1;
//     } else {
//         memo[i][j] = std::max(dp(s1, i + 1, s2, j), dp(s1, i, s2, j + 1));
//     }

//     return memo[i][j];
// }

// int longestCommonSubsequence(const string &s1, const string &s2) {
//     int m = s1.length(), n = s2.length();
//     memo = vector<vector<int>>(m, vector<int>(n, -1));

//     return dp(s1, 0, s2, 0);
// }

// 递推方式
int longestCommonSubsequence(const string &s1, const string &s2) {
    int m = s1.size(), n = s2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = std::max(dp[i][j - 1], dp[i - 1][j]);
            }
        }
    }

    return dp[m][n];
}

// 0-1 背包
int knapsack(int W, int N, vector<int> &weights, vector<int> &vals) {
    vector<vector<int>> dp(N + 1, vector<int>(W + 1, 0));
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= W; ++j) {
            if (j - weights[i - 1] < 0) {
                dp[i][j] = dp[i - 1][w];
            } else {
                dp[i][j] = std::max(dp[i - 1][j - weights[i - 1]] + vals[i - 1], dp[i - 1][j]);
            }
        }
    }

    return dp[N][W];
}

// 子集背包
int canPartition(vector<int> &nums) {
    int sum = 0;
    for (const int &num : nums) {
        sum += num;
    }

    if (sum % 2 != 0) {
        return false;
    }

    int n = nums.size();
    sum /= 2;

    // base case
    vector<vector<bool>> dp(n + 1, vector<bool>(sum + 1, false));
    for (int i = 0; i <= n; ++i) {
        dp[i][0] = true;
    }

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= sum; ++j) {
            if (j - nums[i - 1] < 0) {
                dp[i][j] = dp[i - 1][j];
            } else {
                dp[i][j] = dp[i - 1][j] || dp[i - 1][j - nums[i - 1]];
            }
        }
    }

    return dp[n][sum];
}

// 子集背包
// 状态压缩
int canPartition(vector<int> &nums) {
    int sum = 0;
    for (const int &num : nums) {
        sum += num;
    }

    if (sum % 2 != 0) {
        return false;
    }

    int n = nums.size();
    sum /= 2;

    // 初始化
    vector<bool> dp(sum + 1, false);

    // base case
    dp[0] = true;

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= sum; ++j) {
            if (j - nums[i] >= 0) {
                dp[j] = dp[j] || dp[j - nums[i]];
            }
        }
    }

    return dp[sum];
}

// 完全背包
int change(int amount, vector<int> &coins) {
    int n = coins.size();

    vector<vector<int>> dp(n + 1, vector<int>(amount + 1, 0));

    // base case
    for (int i = 0; i <= n; ++i) {
        dp[i][0] = 1;
    }

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= amount; ++j) {
            if (j - coins[i - 1] >= 0) {
                dp[i][j] = dp[i - 1][j] + dp[i][j - coins[i - 1]];
            } else {
                dp[i][j] = dp[i - 1][j];
            }
        }
    }

    return dp[n][amount];
}
