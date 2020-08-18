
/** @file main.h
  * @brief 
  * @author yingchun.xu
  * @date 2020/7/24
  */

#include <stack>
#include <exception>
#include <iostream>
#include <stdexcept>

using namespace std;

class EmptyQueueException : std::exception {
public:
    const char *what() const _NOEXCEPT override {
        return "empty queue";
    }
};

template<typename T>
class CQueue {
public:
    CQueue() = default;

    ~CQueue() = default;

    void append_tail(const T &node);

    //T delete_head() throw(EmptyQueueException);
    T delete_head() noexcept(false);
    //T delete_head() throw(std::bad_exception);

private:
    stack<T> stack1_;
    stack<T> stack2_;
};

template<typename T>
void CQueue<T>::append_tail(const T &node) {
    stack1_.push(node);
}

template<typename T>
T CQueue<T>::delete_head() noexcept(false) {
    if (stack2_.empty()) {
        while (!stack1_.empty()) {
            T &em = stack1_.top();
            stack1_.pop();
            stack2_.push(em);
        }
    }

    if (stack2_.empty()) {
        //throw std::runtime_error("empty queue");
        throw EmptyQueueException();
    }

    T em = stack2_.top();
    stack2_.pop();
    return em;
}

int main() {
    CQueue<int> queue;
    queue.append_tail(9);
    queue.append_tail(10);
    queue.append_tail(11);

    std::cout << "1:" << queue.delete_head() << std::endl;
    std::cout << "2:" << queue.delete_head() << std::endl;
    std::cout << "3:" << queue.delete_head() << std::endl;

    try {
        std::cout << "4:" << queue.delete_head() << std::endl;
        //} catch (const std::bad_exception &e) {
    } catch (const EmptyQueueException &e) {
        std::cout << "Caught Exception: " << e.what() << std::endl;
    }

    return 0;
}