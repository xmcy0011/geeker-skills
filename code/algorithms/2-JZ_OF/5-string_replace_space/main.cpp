/** @file main.h
  * @brief 替换字符串空格
  *
  * 题目：替换空格
  * 请实现一个函数，把字符串的每个空格，替换成"%20"。例如，
  * 请输入"We are happy."，则输出"We%20are%20happy."。
  *
  * @author yingchun.xu
  * @date 2020/7/23
  */

#include <memory>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;


/**
 * 自己写的，还有bug
 * 空间复杂度：O(n)
 * 时间复杂度：O(n)
 */
string replace_space(char arr[], const char *str) {
    int space_num = 0;
    int origin_len = 0;
    // 1.找到有多少个空格
    while (arr[origin_len] != '\0') {
        if (arr[origin_len] == ' ') {
            ++space_num;
        }
        ++origin_len;
    }

    // 2.初始化1个临时缓冲区
    int new_len = origin_len + space_num * 2;
    char dest[new_len + 1];
    dest[new_len--] = '\0';

    // 3.从后往前替换，避免多余的字符串移动
    for (int i = origin_len - 1; i >= 0; --i) {
        if (arr[i] == ' ') {
            dest[new_len--] = '%';
            dest[new_len--] = '2';
            dest[new_len--] = '0';
        } else {
            dest[new_len--] = arr[i];
        }
    }
    string result = string(dest);
    return result;
}

/**
 * 书上的优化操作，时间O(n)，空间O(1)
 */
void replace_space(char origin[], int len, const char *str) {
    if (origin == nullptr || len <= 0) {
        return;
    }

    // 统计原来的
    int space_num = 0;
    int origin_len = 0;
    while (origin[origin_len] != '\0') {
        if (origin[origin_len] == ' ') {
            ++space_num;
        }
        ++origin_len;
    }

    // 计算替换后的长度
    int new_len = origin_len + space_num * 2;
    if (new_len > len) {
        return;
    }

    origin[new_len] = '\0';
    while (origin_len >= 0 && new_len > origin_len) {
        if (origin[origin_len] == ' ') {
            origin[new_len--] = '%';
            origin[new_len--] = '2';
            origin[new_len--] = '0';
        } else {
            origin[new_len--] = origin[origin_len];
        }

        --origin_len;
    }
}

int main() {
    char temp[200] = {"We are happy."};
    replace_space(temp, 200, "%20");
    //char arr[] = {"We are happy."};
    //string temp = replace_space(arr, "%20");
    cout << "output is :" << temp << endl;
    return 0;
}