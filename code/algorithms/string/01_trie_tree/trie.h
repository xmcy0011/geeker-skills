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
#include <unordered_set>

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
    std::unordered_map<char, TrieNode *> subNodes_;
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

/** @class Trie
  * @brief trie树算法过滤敏感词。
  *
  * 1. 忽略大小写字母
  * 2. 支持中文敏感词
  * 3. 停顿词只支持特殊符号，暂不支持中文
  * 4. 暂不支持全角和半角
  *
  */
class Trie {
    /** Initialize your data structure here. */
public:
    Trie();

    ~Trie();

    // delete
    Trie(const Trie &that) = delete;

    Trie &operator=(const Trie &that) = delete;

    /** @fn loadFromFile
      * @brief 从文件加载敏感词列表，文件utf8格式，一个敏感词单独一行
      * @param [in]file_name: file full path
      * @return void
      */
    void loadFromFile(const std::string &file_name);

    /** @fn loadStopWord
      * @brief 加载停顿词从指定的文件
      * @param [in]file_name:  file full path
      * @return void
      */
    void loadStopWord(const std::string &file_name);

    /** @fn insert
      * @brief Inserts a word into the trie
      * @param [in]word: utf8 word
      * @return void
      */
    void insert(const std::string &word);

    /** @fn search
      * @brief Returns if the word is in the trie
      * @param [in]word: utf8 word
      * @return bool result
      */
    bool search(std::string word);

    /** @fn startsWith
      * @brief Returns if there is any word in the trie that starts with the given prefix
      * @param [in]prefix: prefix
      * @return bool result
      */
    bool startsWith(std::string &prefix);

    /** @fn getSensitive
      * @brief 过滤敏感词并返回敏感词命中位置和信息
      * @param [in]word: 原始字符串，utf8格式，支持中文
      * @return 命中敏感词信息
      */
    std::set<SensitiveWord> getSensitive(std::string word);

    /** @fn replaceSensitive
      * @brief 替换敏感词为*
      * @param [in]word: 字符串内容
      * @return 替换后的文本
      */
    std::string replaceSensitive(const std::string &word);

private:
    int getSensitiveLength(std::string text, int startIndex);

    TrieNode *root_;
    std::unordered_set<char> stop_words_;
};