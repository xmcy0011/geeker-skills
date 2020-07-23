/** @file main.h
  * @brief
  *
  * 题目：二维数组中的查找。
  * 在一个二维数组中，每一行都按照从左到右递增的顺序排序，每一列都按照从上到下的顺序排序。
  * 请完成一个函数，输入这样的一个二维数组和一个整数，判断数组中是否含有该整数。
  *
  * 比如：
  * 1 2 8 9
  * 2 4 9 12
  * 4 7 10 13
  * 6 8 11 15
  *
  * 此时输入7，则返回true。输入5，则返回false
  *
  * @author yingchun.xu
  * @date 2020/7/22
  */

#include <iostream>

// 最基础的NxN遍历
bool findInMatrix(int *arr, int row, int column, int num) {
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < column; ++j) {
            int val = arr[i * row + j];
            std::cout << val << std::endl;
            if (val == num) {
                return true;
            }
        }
    }
    return false;
}

/**
 * 利用
 */
bool findInMatrix2(const int *arr, int rows, int columns, int num) {
    bool is_find = false;
    if (arr == nullptr || rows < 0 || columns < 0) {
        return false;
    }

    int col = columns - 1;
    // 这是根据书里的思路，自己写的代码
#if 0
    for (int i = 0; i < row; ++i) {
        for (int j = col; j >= 0; --j) {
            // 先找最右上角找，这是一行中最大的数字，也是1列中最小的数字。
            int val = arr[i * row + j];
            if (val == num) {
                return true;
            } else if (val > num) {// 如果这个数字大于num，则这一列永远不用找了，因为这一列后续的数都比它大
                --col;
            } else {// 如果比7小，则这一行不用找了，因为剩下的一定小于num，需要去下一行
                break;
            }
        }
    }
#endif

    // 书上的思路有更好的可读性
    int row = 0;
    while (row < rows && col > 0) {
        int val = arr[row * rows + col];
        if (val == num) {
            is_find = true;
            break;
        } else if (val > num) {
            --col;
        } else {
            ++row;
        }
    }
    return is_find;
}

int main() {
    int matrix[][4] = {{1, 2, 8,  9},
                       {2, 4, 9,  12},
                       {4, 7, 10, 13},
                       {6, 8, 11, 15}};
    int num = 7; // 5,7,11
    bool ret = findInMatrix2((int *) matrix, 4, 4, num);
    std::cout << num << " exist in matrix? " << ret << std::endl;
    return 0;
}