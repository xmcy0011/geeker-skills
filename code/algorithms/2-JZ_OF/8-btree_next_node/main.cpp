/** @file main.h
  * @brief 二叉树的下一个节点
  *
  * 题目：给定一颗二叉树和其中的一个节点，如何找出中序遍历序列的下一个节点？树中的节点除了有两个分别指向左、右子节点的指针，
  * 还有一个指向父节点的指针
  *
  * 保存为.dot，然后使用vscode安装Graphviz Preview和Graphviz (dot) language support即可可视化查看二叉树。
digraph G {
    a -> b;
    b -> d;
    b -> e;
    e -> h;
    e -> i;
    a -> c;
    c -> f;
    c -> g;
}
  *
  * 如上图二叉树的中序遍历序列是{d,b,h,e,i,a,f,c,g}
  *
  * 五毒神掌：
  * 1.先思考5分钟。解法1：中序遍历一遍加入到一个临时数组，再编译找到这个节点，下一个节点即是索引+1
  *
  * @author yingchun.xu
  * @date 2020/8/10
  */

#include <cstdio>
#include <iostream>

using namespace std;

struct BinaryTreeNode {
    BinaryTreeNode *left_;
    BinaryTreeNode *right_;
    BinaryTreeNode *parent_;
    int value_;
};

BinaryTreeNode *create_node(int value) {
    BinaryTreeNode *node = new BinaryTreeNode();
    node->value_ = value;
    node->left_ = nullptr;
    node->right_ = nullptr;
    node->parent_ = nullptr;
    return node;
}

void connect_node(BinaryTreeNode *parent, BinaryTreeNode *left, BinaryTreeNode *right) {
    if (parent != nullptr) {
        parent->left_ = left;
        parent->right_ = right;

        if (left != nullptr) {
            left->parent_ = parent;
        }
        if (right != nullptr) {
            right->parent_ = parent;
        }
    }
}

void destroy_tree(BinaryTreeNode *root) {
    if (root != nullptr) {
        delete root;
        root = nullptr;

        destroy_tree(root->left_);
        destroy_tree(root->right_);
    }
}

BinaryTreeNode *get_next(BinaryTreeNode *node) {
    // 1. 如果给定的节点有右子树，则下一个节点是其右子树的最左子节点。
    // 2. 如果没有右子树。
    //  1）是左子节点。下一节点就是父节点
    //  2）是右子节点。向上遍历，找到一个节点是其父节点的左子节点，则这个节点的父节点就是下一节点

    if (node == nullptr) {
        return nullptr;
    }

    BinaryTreeNode *next = nullptr;

    if (node->right_ != nullptr) {
        BinaryTreeNode *right = node->right_;
        // 不断的去找最左的子节点
        while (right->left_ != nullptr) {
            right = right->left_;
        }
        next = right;
    } else if (node->parent_ != nullptr) {
        // 没有右子树是左子节点，下一个节点就是父节点
        if (node == node->parent_->left_) {
            next = node->parent_;
        } else {
            // 没有右子树是右子节点，下一个节点向上找
            // 找到一个节点是其父节点的左子节点，则这个节点的父节点就是下一节点
            BinaryTreeNode *cur = node;
            BinaryTreeNode *parent = node->parent_;
            while (parent != nullptr && parent->right_ == cur) {
                cur = parent;
                parent = parent->parent_;
            }
            next = parent;
        }
    }

    return next;
}

void test(string name, BinaryTreeNode *node, BinaryTreeNode *expected) {
    if (!name.empty()) {
        cout << "begin:" << name;
    }

    BinaryTreeNode *result = get_next(node);
    if (result != expected) {
        cout << ",failed,cur:" << node->value_ << ",expected:" << expected->value_ << endl;
    } else {
        cout << ",success" << endl;
    }
}

//            8
//        6      10
//       5 7    9  11
void test1_7() {
    BinaryTreeNode *node8 = create_node(8);
    BinaryTreeNode *node6 = create_node(6);
    BinaryTreeNode *node10 = create_node(10);
    BinaryTreeNode *node5 = create_node(5);
    BinaryTreeNode *node7 = create_node(7);
    BinaryTreeNode *node9 = create_node(9);
    BinaryTreeNode *node11 = create_node(11);

    connect_node(node8, node6, node10);
    connect_node(node6, node5, node7);
    connect_node(node10, node9, node11);

    test("test1", node8, node9);
    test("test2", node6, node7);
    test("test3", node5, node6);
    test("test4", node7, node8);
    test("test5", node10, node11);
    test("test6", node9, node10);
    test("test7", node11, nullptr);

    destroy_tree(node8);
}


int main() {
    test1_7();

    return 0;
}