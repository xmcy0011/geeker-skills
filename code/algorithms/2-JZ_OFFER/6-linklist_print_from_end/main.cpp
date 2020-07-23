/** @file main.h
  * @brief 从尾到头打印链表
  *
  * 题目：
  * 输入一个链表的头节点，从尾到头反过来打印出每个节点的值。链表定义如下：
struct ListNode {
    int key_;
    ListNode *next_;
};
  *
  * @author yingchun.xu
  * @date 2020/7/23
  */

#include <stack>
#include <iostream>

struct ListNode {
    int key_;
    ListNode *next_;
};

// 解法1：先访问了头却后输出，即先进后出，可以使用stack来实现
void print_node(ListNode *header) {
    using namespace std;

    if (header == nullptr) {
        return;
    }

    std::stack<ListNode *> stack;
    ListNode *next = header;
    while (next != nullptr) {
        stack.push(next);
        next = next->next_;
    }

    while (!stack.empty()) {
        ListNode *node = stack.top();
        cout << "key: " << node->key_ << endl;
        stack.pop();
    }
}

// 解法2：递归，每次都打印后面的元素。但是推荐解法1，因为如果链表很长的话，可能造成函数调用堆栈溢出的错误
void print_node_reverse(ListNode *header) {
    if (header != nullptr) {
        if (header->next_ != nullptr) {
            print_node_reverse(header->next_);
        }

        std::cout << "key: " << header->key_ << std::endl;
    }
}

int main() {
    ListNode *header = new ListNode();
    header->key_ = 0;
    header->next_ = nullptr;
    ListNode *pre_node = header;

    for (int i = 1; i < 10; ++i) {
        ListNode *node = new ListNode();
        node->key_ = i;
        node->next_ = nullptr;

        pre_node->next_ = node;
        pre_node = node;
    }

    //print_node(header);
    print_node_reverse(header);

    ListNode *next = header;
    while (next != nullptr) {
        ListNode *cur = next;
        next = next->next_;

        cur->next_ = nullptr;
        delete cur;
    }

    return 0;
}