/** @file main.h
  * @brief 重建二叉树
  *
  * 题目：
  * 输入某二叉树的前序遍历和中序遍历的结果，请重建该二叉树。假设输入的前序遍历和中旬遍历的结果中
  * 都不含重复的数字。例如，输入前序遍历序列{1,2,4,7,35,6,8}和中序遍历序列{4,7,2,1,5,3,8,6}，
  * 则重建如图2.6所示的二叉树并输出它的头节点。二叉树节点的定义如下：
struct BinaryTreeNode {
    int value_;
    BinaryTreeNode *left_;
    BinaryTreeNode *right_;
};
  * @author yingchun.xu
  * @date 2020/7/23
  */

#include <cstdio>
#include <iostream>

struct BinaryTreeNode {
    int value_;
    BinaryTreeNode *left_;
    BinaryTreeNode *right_;
};


// 复习一下。
// 概念（数据结构课程华中大清华大学严蔚敏教材 https://www.bilibili.com/video/BV15E411V7S2?p=28）：
// - 节点的度
// - 叶子
// - 双亲与孩子
// - 兄弟
// - 祖先
// - 树的度
// - 节点的层次
// - 树的深度
// - 有序树与无序树
// - 森林

// 二叉树：二叉树或为空树，或是有一个根节点加上两颗分别成为左子树和右子树的、互补相交的二叉树组成。

// 树转二叉树
// 存储结构：链式存储（见上述定义）：左 长子，右 下一兄弟。
// 方法：加线（兄弟相连）——抹线（长兄为父）——旋转（孩子靠左）

// 二叉树还原为树：
// 逆操作，把所有右孩子变为兄弟。

// 二叉树性质：
// 1. 在二叉树的第i层上至多有2的（i-1）次方个节点。
// 2. 深度为k的二叉树至多有2的（k次方）-1个节点
// 3. 对于任何一颗二叉树，如果其终端节点树为n0，度为2的节点树为n2，则n0=n2+1
// 4. 具有n(n>=0)个结点的完全二叉树的深度为[log2(n)]+1
// 5. 略

// 创建链式二叉树：
BinaryTreeNode *build_tree() {
    char c;
    BinaryTreeNode *node = nullptr;

    c = static_cast<char>(getchar());
    if (c == '0') {
        std::cout << "end" << std::endl;
        return (0);
    }

    node = new BinaryTreeNode();
    node->value_ = c;
    node->left_ = build_tree();
    node->right_ = build_tree();
    return node;
}

int main() {
    std::cout << "please input a letters, 0 means end" << std::endl;
    build_tree();

    return 0;
}