
/** @file main.h
  * @brief 
  * @author yingchun.xu
  * @date 2020/7/24
  */

#include <stack>

using namespace std;

template<typename T>
class CQueue {
public:
    CQueue() {}

    ~CQueue() {}

    void append_tail(const T &node);

    T delete_head();

private:
    stack<T> stack1_;
    stack<T> stack2_;
};

template<typename T>
void CQueue<T>::append_tail(const T &node) {
    stack1_.push(node);
}

template<typename T>
T CQueue<T>::delete_head() {
    if (stack2_.empty()) {
        while (!stack1_.empty()) {
            T &em = stack1_.top();
            stack1_.pop();
            stack2_.push(em);
        }
    }

    if (!stack2_.empty()) {
        T &em = stack2_.top();
        stack2_.pop();
        return em;
    }

    return nullptr;
}


int main() {
    return 0;
}