/** @file stl_algorithm_transform.h
  * @brief 
  * @author yingchun.xu
  * @date 2020/7/19
  */

#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <functional>
#include <list>

using namespace std;

// 自定义函数对象
template<typename T>
class Square {
public:
    // 重载函数运算符
    T operator()(T &t) {
        return t * t;
    }
};

int main() {
    const int n = 5;
    vector<int> s(n); // 容器
    for (int i = 0; i < n; i++)
        cin >> s[i];
    // 算法：transform
    // 迭代器：s.begin()
    // 函数对象：negate<int>()，取相反数
    //transform(s.begin(), s.end(), ostream_iterator<int>(cout, " "), negate<int>());
    transform(s.begin(), s.end(), ostream_iterator<int>(cout, " "), Square<int>());

    list<int> coll;
    // const_iterator 只读
    for (list<int>::const_iterator pos = coll.begin(); pos != coll.end(); ++pos) {

    }
    // auto使迭代器失去常量性，可能引发计划外的赋值风险
    for (auto pos = coll.begin(); pos != coll.end(); ++pos) {

    }
    // 更好的方式是使用cbegin和cend代替，返回const_iterator
    for (auto pos = coll.cbegin(); pos != coll.cend(); ++pos) {

    }
    // Range-Base for
    // 声明元素为reference很重要，可以避免local copy，如果是只读，加上const更好
    for (const auto &item : coll) {
    }
    // 被展开成
    for (auto pos = coll.begin(), end = coll.end(); pos != end; ++pos) {
        int item = *pos; // local copy
    }

    cout << endl;
    return 0;
}