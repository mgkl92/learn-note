#include <iostream>
#include <vector>

using std::vector;

// 挖洞法
// int partition(vector<int> &data, int left, int right) {
//     int pivot = data[left];
//     while (left < right) {
//         while (left < right && pivot <= data[right]) {
//             --right;
//         }

//         if (left < right) {
//             data[left++] = data[right];
//         }

//         while (left < right && pivot > data[left]) {
//             ++left;
//         }

//         if (left < right) {
//             data[right--] = data[left];
//         }
//     }
//     data[left] = pivot;
//     return left;
// }

int partition(vector<int> &data, int left, int right) {
    int temp = data[left];
    int i = left, j = right;

    while (i < j) {
        while (i < j && temp <= data[j]) {
            --j;
        }
        while (i < j && temp >= data[i]) {
            ++i;
        }
        if (i < j) {
            std::swap(data[i], data[j]);
        }
    }
    std::swap(data[left], data[i]);
    return i;
}

int quick_sort(vector<int> &data, int left, int right) {
    if (left >= right) {
        return 1;
    }
    int middle = 0;

    middle = partition(data, left, right);
    quick_sort(data, left, middle - 1);
    quick_sort(data, middle + 1, right);

    return 0;
}

int main(int argc, char const *argv[]) {
    vector<int> data = {1, 4, 2, 7, 8, 20};

    quick_sort(data, 0, data.size() - 1);

    for (const int &val : data) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    return 0;
}
