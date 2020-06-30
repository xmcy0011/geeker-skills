#include <stdio.h>

/**
 * 试用C或类C语言编写一高效算法，将一顺序存储的线性表（设元素为整型）中
 * 所有零元素向表尾集中，其他元素则顺序向表头方向集中。
 * 时间复杂度：o(n)
 * 空间复杂度：o(1)
 */
void sort1(int *arr, int length) {
  int count = 0, i, j = 0;
  int n = length;

  for (i = 0; i < length; i++) {
    if (arr[i] != 0) {
      arr[j++] = arr[i];
    } else {
      count++;
    }
  }

  for (i = n - count; i < n; i++) {
    arr[i] = 0;
  }
}

void sort2(int *arr, int length) {
  int i = 0, j = 0, temp;
  while (i < length) {
    temp = arr[i];
    arr[i++] = 0;
    if (temp)
      arr[j++] = temp;
  }
}

void print_arr(int *arr, int length) {
  for (int i = 0; i < 10; i++) {
    printf("%d=%d \n", i, arr[i]);
  }
}

int main(int argc, char **argv) {
  int test[10] = {3, 42, 1, 0, 6, 0, 0, 34, 2};
  // sort1(test, 10);
  sort2(test, 10);
  print_arr(test, 10);
  return 0;
}