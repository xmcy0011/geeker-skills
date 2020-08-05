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

 * 保存为.dot，然后使用vscode安装Graphviz Preview和Graphviz (dot) language support即可可视化查看二叉树。
digraph G {
    1 -> 2;
    2 -> 4;
    4 -> 7;
    1 -> 3;
    3 -> 5;
    3 -> 6;
    6 -> 8;
}

  * @author yingchun.xu
  * @date 2020/7/23
  */

#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <vector>

using namespace std;

struct BinaryTreeNode {
    char value_;
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

// 二叉树：二叉树或为空树，或是有一个根节点加上两颗分别称为左子树和右子树的、互不相交的二叉树组成。

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

// 先序、中序、后序的特点：
// 先序：根 -> Left -> Right，即先访问根，然后是左子树，再是右子树。所以第一个输出的是根节点。
// 中序：Left -> 根 -> Right，即先访问左子树，然后是根，再是右子树。所以根节点在中间。
// 后序：Left -> Right -> 根，最后一个节点是根。
// 总结：先序和后序可以确定根节点，中序可以确定左子树和右子树。

// 创建链式二叉树
// 输入：12300400600
// 解读：1的左孩子是2，2的左孩子是3，3的左右孩子都是0。2的右孩子是4，4的左右孩子都是0。1的右孩子是6，6的左右孩子都是0。
// 输出：
// digraph G {
//  1 -> 2;
//  2 -> 3;
//  2 -> 4;
//  1 -> 6;
// }
//
// 再延伸一个题：https://blog.csdn.net/eebaicai/article/details/89788098
// 通过二叉树的2种序列，可唯一确定这棵树，并得出另一种序列。
// 例：已知二叉序的前序遍历是ABHFDECKG，中序遍历序列是HBDFAEKCG，它的后序遍历是？
//
BinaryTreeNode *build_tree() {
    char c;
    BinaryTreeNode *node = nullptr;

    c = static_cast<char>(getchar());
    if (c == '0' || c == '\n') {
        std::cout << "end" << std::endl;
        return nullptr;
    }

    node = new BinaryTreeNode();
    node->value_ = c;
    node->left_ = build_tree();
    node->right_ = build_tree();
    return node;
}

//BinaryTreeNode *build_tree(const vector<int> &q, const vector<int> &h) {
//
//}

void printNode(const BinaryTreeNode *root, ostringstream &buffer) {
    if (root == nullptr) {
        return;
    }

    printNode(root->left_, buffer);
    printNode(root->right_, buffer);

    if (root->left_ != nullptr) {
        buffer << root->value_ << " -> " << root->left_->value_ << ";\n";
    } else {
        buffer << root->value_ << " -> nil;\n";
    }
    if (root->right_ != nullptr) {
        buffer << root->value_ << " -> " << root->right_->value_ << ";\n";
    } else {
        buffer << root->value_ << " -> nil;\n";
    }
}

void printIntoGraphviz(const BinaryTreeNode *root, ostringstream &buffer) {
    buffer << "digraph G {\n";
    printNode(root, buffer);
    buffer << "}";
}

void saveToGraphviz(ostringstream &buffer) {
    remove("binaryTreeNode.dot");
    std::shared_ptr<FILE *> handle = std::make_shared<FILE *>(fopen("binaryTreeNode.dot", "w+"));

    if (handle != nullptr) {
        fprintf(*handle, "%s", buffer.str().c_str());
        fclose(*handle);
    }
}

void test_build_tree() {
    std::cout << "please input a letters, 0 means end" << std::endl;
    std::cout << "example:12300400600" << std::endl;
    BinaryTreeNode *root = build_tree();

    // append output string stream
    ostringstream buffer("", ::ios_base::ate);
    printIntoGraphviz(root, buffer);
    printf("%s", buffer.str().c_str());
    saveToGraphviz(buffer);
}

int main() {
    // 测试用例1：12300400600、123000600
    // 测试用例2：
    //  输出：A BHFD ECKG
    //  输出：HBDF A EKCG
    //  输入1：ABH00FD000E0CK00G00
    test_build_tree();

    return 0;
}