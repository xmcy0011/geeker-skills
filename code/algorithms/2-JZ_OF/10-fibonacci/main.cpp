/** @file main.h
  * @brief 斐波那契数列
  *
  * 题目：求斐波那契数列的第n项。
  * 写一个函数，输入n，求斐波那契（fibonacci）数列的第n项。斐波那契数列的定义如下：
  *
  * f(n) = { 0  n=0
  *        { 1  n=1
  *        { f(n-1) + f(n-2)
  *
  *
  * 题目二：青蛙跳台阶。
  * 一只青蛙一次可以跳上1级台阶，也可以跳上2级台阶。求该青蛙跳上一个n级的台阶总共有多少种跳法？
  *
  * 青蛙跳台阶扩展：
  * 一只青蛙一次可以跳上1级台阶，也可以跳上2级台阶……它也可以跳上n级，此时青蛙跳上一个n级的台阶总共有多少种跳法？用数学归纳法可以证明f(n)=2的n-1次方
  *
  * @author yingchun.xu
  * @date 2020/8/11
  */

#include <iostream>

// 解法1（递归）：大量的重复计算，要求得f(10)，则需要求得f(9)和f(8)……。从fibonacci(20)开始，就很慢了，fibonacci(100)简直慢到爆炸
//                    f(10)
//            f(9)            f(8)
//       f(8)      f(7)    f(7)  f(6)
//     f(7) f(6) f(6) f(5)
int64_t fibonacci(int64_t n) {
    if (n <= 0) {
        return 0;
    }
    if (n == 1) {
        return 1;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// 解法2（循环）：从下往上计算，首先根据f(0)和f(1)算出f(2)，再根据f(1)和f(2)算出f(3)，以此类推，就可以算出第n项了。
//    时间复杂度：O(n)
int64_t fibonacci_for(int64_t n) {
    int result[] = {0, 1};
    if (n < 2) {
        return result[n];
    }

    int64_t pre_pre = 0; // f(n-2)
    int64_t pre = 1;     // f(n-1)
    int64_t fibo = 0;

    for (int64_t i = 2; i <= n; ++i) {
        fibo = pre_pre + pre; // f(n) = f(n-1)+f(n-2);

        pre_pre = pre;
        pre = fibo;
    }

    return fibo;
}

int main() {
    time_t pre = time(nullptr);
    // std::cout << fibonacci(60) << std::endl;
    // 结果：3736710778780434371，如果是递归，等睡醒了可能都还没有出结果，或者报错了（堆栈溢出）
    std::cout << fibonacci_for(100) << std::endl;

    // 题目二：青蛙跳台阶，实际上就是斐波那契数列。
    // 1. 如果只有1级台阶，跳法=1
    // 2. 如果有2级台阶，跳法=2，一次跳一级，一次跳2级。
    // 3. 把n级台阶的跳法记成n的函数f(n)，当n>2时，第一次跳的时候就两种不同的选择：一是第一次只跳1级，此时跳法数目等于后面剩下的n-1级台阶的跳法数目，
    // 即为f(n-1)；二是第一次跳2级，此时跳法数目等于后面剩下的n-2级台阶的跳法数目，即为f(n-2)。
    // 因此，n级台阶的跳法总数f(n)=f(n-1)+f(n-2)。

    time_t cur = time(nullptr);

    std::cout << "pre:" << pre << ",cur:" << cur << std::endl;
    std::cout << "diff:" << ::abs(cur - pre) << std::endl;

    return 0;
}