#include <vector>
#include <queue>
#include <string>
#include <unordered_map>
#include <climits>

using std::vector;
using std::priority_queue;
using std::string;
using std::unordered_map;

struct ListNode {
    int val;
    ListNode *next;
};

// 合并两个链表
// 非递归方法
ListNode *merge(ListNode *list1, ListNode *list2) {
    ListNode dummy = {0, nullptr}, *p = &dummy;
    while (list1 && list2) {
        ListNode *temp;
        if ((list1->val) < (list2->val)) {
            temp = list1;
            list1 = temp->next;
        } else {
            temp = list2;
            list2->next;
        }

        // 尾插法
        p->next = temp;
        p = temp;
    }

    if (list1) {
        p->next = list1;
    }

    if (list2) {
        p->next = list2;
    }

    return dummy.next;
}

ListNode *mergeRecursive(ListNode *list1, ListNode *list2) {
    if (!list1) {
        return list2;
    }

    if (!list2) {
        return list1;
    }

    if ((list1->val) < (list2->val)) {
        list1->next = mergeRecursive(list1->next, list2);
        return list1;
    } else {
        list2->next = mergeRecursive(list1, list2->next);
        return list2;
    }
}

// 查找第 k 个元素
ListNode *findFromEnd(ListNode *head, int k) {
    ListNode *p = head, *q = head;
    // p 指向第 k 个元素
    while (k--) {
        p = p->next;
    }

    // q 指向第 n - k 个元素
    // 即倒数第 k 个元素(n - k + 1)的前一个元素
    while (p->next) {
        p = p->next;
        q = q->next;
    }

    // 倒数第 k 个元素
    return q->next;
}

// 非递减序合并 k 个链表
ListNode *mergeKLists(vector<ListNode *> &lists) {
    ListNode dummy = {0, nullptr}, *rear = &dummy;
    auto cmp = [](ListNode *a, ListNode *b) {
        return (a->val) > (b->val);
    };

    priority_queue<ListNode *, vector<ListNode *>, decltype(cmp)> pq(cmp);

    for (auto list : lists) {
        if (list) {
            pq.push(list);
        }
    }

    while (!pq.empty()) {
        ListNode *q = pq.top();
        pq.pop();

        rear->next = q;
        rear = q;

        if (q->next) {
            pq.push(q->next);
        }
    }
    rear->next = nullptr;

    return dummy.next;
}

// 判断链表是否含环
bool hasCycle(ListNode *head) {
    ListNode *slow = head, *fast = head;
    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast) {
            return true;
        }
    }

    return false;
}

// 检测环入口
ListNode *detectCycle(ListNode *head) {
    if (!head || !head->next) {
        return nullptr;
    }

    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast) {
            break;
        }
    }

    slow = head;
    while (slow != fast) {
        fast = fast->next;
        slow = slow->next;
    }

    return slow;
}

// 删除数组中的重复元素
int removeDulpicates(vector<int> &nums) {
    int n = nums.size();

    if (n == 0) {
        return 0;
    }

    // i 为无重复数组中最后一个元素的索引
    int i = 0, j = 0;
    while (j < n) {
        if (nums[j] != nums[i]) {
            nums[++i] = nums[j];
        }
        j++;
    }

    // 元素数量 = 索引 + 1
    return i + 1;
}

// 删除链表中的重复元素
ListNode *removeDulpicates(ListNode *head) {
    if (!head) {
        return nullptr;
    }

    ListNode *slow = head, *fast = head;
    while (fast->next) {
        ListNode *temp = fast->next;
        if (temp->val != slow->val) {
            // 尾插法
            slow->next = temp;
            slow = temp;
        }
        fast = temp;
    }

    slow->next = nullptr;
    return head;
}

// 两数之和
vector<int> twoSum(vector<int> &nums, int tgt) {
    int left = 0, right = nums.size() - 1;
    while (left < right) {
        int sum = nums[left] + nums[right];
        if (sum == tgt) {
            return {left, right};
        } else if (sum < tgt) {
            ++left;
        } else {
            --right;
        }
    }

    return {-1, -1};
}

// 反转数组
void reverseString(vector<char> &s) {
    int left = 0, right = s.size() - 1;
    while (left < right) {
        std::swap(s[left], s[right]);
        ++left, --right;
    }
}

// 回文串判断
bool isPalindrome(const string &s) {
    int left = 0, right = s.size() - 1;
    while (left < right) {
        if (s[left] != s[right]) {
            return false;
        }
        ++left, --right;
    }
    return true;
}

// 满足条件的最小窗口
string minWindow(const string &s, const string &t) {
    unordered_map<char, int> needs, windows;
    for (const char &c : t) {
        ++needs[c];
    }

    int left = 0, right = 0;
    int valid = 0;
    int start = 0, len = INT_MAX;
    while (right < s.size()) {
        // 将字符 c 移入窗口（扩大窗口）
        char c = s[right++];
        if (needs.count(c)) {
            ++windows[c];
            if (windows[c] == needs[c]) {
                ++valid;
            }
        }

        while (valid == needs.size()) {
            if (right - left < len) {
                start = left;
                len = right - left;
            }
            // 将字符 d 移除窗口（收缩窗口）
            char d = s[left++];
            if (needs.count(d)) {
                if (windows[d] == needs[d]) {
                    --valid;
                }
                --windows[d];
            }
        }
    }

    return len == INT_MAX ? "" : s.substr(start, len);
}