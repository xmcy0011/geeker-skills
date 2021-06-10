/** @file trie.h
  * @brief trie树算法实现的敏感词过滤
  *
  * Trie 树原理及其敏感词过滤的实现: https://www.jianshu.com/p/9919244dd7ad
  * wildfirechat: https://github.com/wildfirechat/server/blob/wildfirechat/broker/src/main/java/win/liyufan/im/SensitiveFilter.java
  * trie: https://github.com/r-lyeh-archived/trie/blob/master/trie.hpp
  *
  * @author teng.qing
  * @date 2021/6/10
  */

#include <set>
#include <string>
#include <unordered_map>

/** @class trie
  * @brief trie树算法实现的敏感词过滤
  */
class TrieNode {
public:
    TrieNode();

    ~TrieNode();

    // 添加子节点
    void addSubNode(const char &c, TrieNode *subNode) { subNodes_[c] = subNode; }

    // 获取子节点
    TrieNode *getSubNode(const char &c) { return subNodes_[c]; }

private:
    // 子节点(key是下级字符，value是下级节点)
    std::unordered_map<uint32_t, TrieNode *> subNodes_;
};

struct SensitiveWord {
    std::string word;
    int startIndex;
    int len;

    friend bool operator<(struct SensitiveWord const &a,
                          struct SensitiveWord const &b) {
        return a.startIndex < b.startIndex;
    }
};

class Trie {
    /** Initialize your data structure here. */
public:
    Trie();

    ~Trie();

    // delete
    Trie(const Trie &that) = delete;

    Trie &operator=(const Trie &that) = delete;

    /** Inserts a word into the trie. */
    void insert(std::string word);

    /** Returns if the word is in the trie. */
    bool search(std::string word);

    /** Returns if there is any word in the trie that starts with the given
     * prefix. */
    bool startsWith(std::string prefix);

    // 获取敏感词列表
    std::set<SensitiveWord> getSensitive(std::string word);

    /** @fn replaceSensitive
      * @brief 替换敏感词为*
      * @param [in&out]word: 字符串内容
      * @return void
      */
    std::string replaceSensitive(const std::string &word);

private:
    int getSensitiveLength(std::string text, int startIndex);

    TrieNode *root_;
};