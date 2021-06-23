/** @file word_node.h
  * @brief 
  * @author teng.qing
  * @date 2021/6/10
  */

#ifndef INC_02_DFA_SENSITIVEWD_FILTER_WORD_NODE_H_
#define INC_02_DFA_SENSITIVEWD_FILTER_WORD_NODE_H_

#include <vector>

/** @class word_node
  * @brief 一个节点
  */
class WordNode {
public:
    explicit WordNode(int value) : value_(value), is_last_(false) {
    }

    WordNode(int value, bool is_last) : value_(value), is_last_(is_last) {

    }

    WordNode *addSubNode(WordNode *subNode) {
        sub_nodes_.push_back(subNode);
        return subNode;
    }

    WordNode *addSubNode(int value, bool is_last) {
        auto *node = new WordNode(value, is_last);
        sub_nodes_.push_back(node);
        return node;
    }

    // 有就直接返回该子节点， 没有就创建添加并返回该子节点
    WordNode *addIfNoExist(int value, bool is_last) {
        if (sub_nodes_.empty()) {
            return addSubNode(value, is_last);
        }

        for (auto &node : sub_nodes_) {
            if (node->value_ == value) {
                if (!node->is_last_ && is_last)
                    node->is_last_ = true;
                return node;
            }
        }
        return addSubNode(new WordNode(value, is_last));
    }

    WordNode *querySub(int value) {
        if (sub_nodes_.empty()) {
            return nullptr;
        }
        for (auto &node : sub_nodes_) {
            if (node->value_ == value)
                return node;
        }
        return nullptr;
    }

    bool isLast() const {
        return is_last_;
    }

    void setLast(bool is_last) {
        is_last_ = is_last;
    }

private:
    int value_;                         // 节点名称
    std::vector<WordNode *> sub_nodes_; // 子节点
    bool is_last_;
};


#endif //INC_02_DFA_SENSITIVEWD_FILTER_WORD_NODE_H_
