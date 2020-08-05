/** @file main.h
  * @brief 数组中查找重复的数字
  *
  * 题目：找出数组中重复的数字。
  * 在一个长度为n的数组里的所有数字都在0～n-1范围内。数组中某些数字是重复的，
  * 但不知道有个数字重复了，也不知道每个数字重复了几次。请找出数组中任意一个重复的数字。
  * 例如，如果输入长度为7的数组{2,3,1,0,2,5,3}，那么对应的输出是重复的数组2或者3。
  *
  * @author yingchun.xu
  * @date 2020/7/22
  */

#include <iostream>
#include <unordered_set>

using namespace std;

/**
 * 通过hash数据结构的键不能重复来实现
 * 时间复杂度：O(n)
 * 空间复杂度：O(n)
 */
bool findDuplicate(int *arr, int len, int *out_duplicate_num) {
    unordered_set<int> set;
    typedef pair<unordered_set<int>::iterator, bool> SetPair;

    for (int i = 0; i < len; ++i) {
        SetPair ret = set.insert(arr[i]);
        if (!ret.second) {
            *out_duplicate_num = arr[i];
            return true;
        } else {
            cout << "insert " << arr[i] << endl;
        }
    }

    return false;
}


int findIndex(int *arr, int index) {
    // 找下标
    int value = arr[index];
    if (value == index) {
        return index;
    }
    return findIndex(arr, value);
}

/**
 * 利用数组下标来比较。
 * 时间复杂度：O(n)
 * 空间复杂度：O(1)
 */
bool findDuplicate2(int arr[], int len, int *out_duplicate_num) {
    // {2,3,1,0,2,5,3}

    // 检查
    for (int i = 0; i < len; ++i) {
        if (arr[i] < 0 || arr[i] > len) {
            cout << "invalid num " << arr[i] << endl;
            return false;
        }
    }

    // 尽管有2重循环，但是每个元素最多只要交换2次就能找到属于他的位置，因此总的时间复杂度是O(n)。
    // 另外，所有输入都是在数组上进行的，不需要分配额外的空间，所有空间复杂度是O(1)
    for (int i = 0; i < len; ++i) {
        while (arr[i] != i) {
            if (arr[i] == arr[arr[i]]) { // 说明，这个下标存在1个重复数字，找到了。
                *out_duplicate_num = arr[i];
                return true;
            } else {// 否则，不一样就交换
                int index = arr[i];
                arr[i] = arr[index];
                arr[index] = index;
            }

            cout << "while i:" << i << ",value:" << arr[i] << endl;
        }

        cout << "for i:" << i << ",value:" << arr[i] << endl;
    }

    return false;
}

int main() {
    int arr[] = {2, 3, 1, 0, 2, 5, 3}; // C++11
    int out_duplicate_num = 0;
    if (findDuplicate2(arr, sizeof(arr) / sizeof(arr[0]), &out_duplicate_num)) {
        cout << "find duplicate num is " << out_duplicate_num << endl;
    } else {
        cout << "not find duplicate num" << endl;
    }
    return 0;
}