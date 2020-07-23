/** @file input_output_iterator.h
  * @brief 输出、输出迭代器
  * @author yingchun.xu
  * @date 2020/7/18
  */

#include <algorithm>
#include <iterator>
#include <vector>
#include <iostream>

using namespace std;

template<class T, class in_iterator, class out_iterator>
void sort(in_iterator first, in_iterator end, out_iterator result) {
    // temp sort vector
    vector<T> s;
    for (; first != end; first++) {
        s.push_back(*first);
    }
}

int main() {
    double score[] = {88, 56, 80, 60, 99, 77};


}