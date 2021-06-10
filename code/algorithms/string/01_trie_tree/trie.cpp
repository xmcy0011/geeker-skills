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

#include "trie.h"
#include <iostream>
#include <fstream>

const char kEndFlag = '&';

TrieNode::TrieNode() = default;

TrieNode::~TrieNode() {
    for (auto i : subNodes_) {
        // std::cout << "delete" << i.first << std::endl;
        delete i.second;
    }
    subNodes_.clear();
}

Trie::Trie() { root_ = new TrieNode(); }

Trie::~Trie() {
    delete root_;
    root_ = nullptr;
}

void Trie::insert(const std::string &word) {
    TrieNode *curNode = root_;
    for (char code : word) {
        TrieNode *subNode = curNode->getSubNode(code);

        // 如果没有这个节点则新建
        if (subNode == nullptr) {
            subNode = new TrieNode();
            curNode->addSubNode(code, subNode);
        }
        // 指向子节点，进入下一循环
        curNode = subNode;
    }
    // 设置结束标识
    curNode->addSubNode(kEndFlag, new TrieNode());
}

bool Trie::search(std::string word) {
    // TrieNode *curNode = root_;
    // for (int i = 0; i < word.length(); i++) {
    //   curNode = curNode->getSubNode(word[i]);
    //   if (curNode == nullptr)
    //     return false;
    // }
    // return curNode->getSubNode(kEndFlag) != nullptr;

    // 转换成小写
    transform(word.begin(), word.end(), word.begin(), ::tolower);
    bool is_contain = false;
    for (int p2 = 0; p2 < word.length(); ++p2) {
        int wordLen = getSensitiveLength(word, p2);
        if (wordLen > 0) {
            is_contain = true;
            break;
        }
    }
    return is_contain;
}

bool Trie::startsWith(std::string &prefix) {
    TrieNode *curNode = root_;
    for (char &i : prefix) {
        curNode = curNode->getSubNode(i);
        if (curNode == nullptr)
            return false;
    }
    return true;
}

std::set<SensitiveWord> Trie::getSensitive(std::string word) {
    // 转换成小写
    transform(word.begin(), word.end(), word.begin(), ::tolower);
    std::set<SensitiveWord> sensitiveSet;

    for (int p2 = 0; p2 < word.length(); ++p2) {
        int wordLen = getSensitiveLength(word, p2);
        if (wordLen > 0) {
            std::string sensitiveWord = word.substr(p2, wordLen);
            SensitiveWord wordObj;
            wordObj.word = sensitiveWord;
            wordObj.startIndex = p2;
            wordObj.len = wordLen;

            sensitiveSet.insert(wordObj);
            p2 = p2 + wordLen - 1;
        }
    }

    return sensitiveSet;
}

int Trie::getSensitiveLength(std::string word, int startIndex) {
    TrieNode *p1 = root_;
    int wordLen = 0;
    bool endFlag = false;

    for (int p3 = startIndex; p3 < word.length(); ++p3) {
        const char &cur = word[p3];
        auto subNode = p1->getSubNode(cur);
        if (subNode == nullptr) {
            // 如果是停顿词，直接往下继续查找
            if (stop_words_.find(cur) != stop_words_.end()) {
                ++wordLen;
                continue;
            }

            break;
        } else {
            ++wordLen;
            // 直到找到尾巴的位置，才认为完整包含敏感词
            if (subNode->getSubNode(kEndFlag)) {
                endFlag = true;
                break;
            } else {
                p1 = subNode;
            }
        }
    }

    // 注意，处理一下没找到尾巴的情况
    if (!endFlag) {
        wordLen = 0;
    }
    return wordLen;
}

/** @fn
  * @brief linux下一个中文占用三个字节,windows占两个字节
  * 参考：https://blog.csdn.net/taiyang1987912/article/details/49736349
  * @param [in]str: 字符串
  * @return
  */
std::string chinese_or_english_append(const std::string &str) {
    std::string replacement;
    //char chinese[4] = {0};
    int chinese_len = 0;
    for (int i = 0; i < str.length(); i++) {
        unsigned char chr = str[i];
        int ret = chr & 0x80;
        if (ret != 0) { // chinese: the top is 1
            if (chr >= 0x80) {
                if (chr >= 0xFC && chr <= 0xFD) {
                    chinese_len = 6;
                } else if (chr >= 0xF8) {
                    chinese_len = 5;
                } else if (chr >= 0xF0) {
                    chinese_len = 4;
                } else if (chr >= 0xE0) {
                    chinese_len = 3;
                } else if (chr >= 0xC0) {
                    chinese_len = 2;
                } else {
                    throw std::exception();
                }
            }
            // 跳过
            i += chinese_len - 1;

            //chinese[0] = str[i];
            //chinese[1] = str[i + 1];
            //chinese[2] = str[i + 2];
        } else /** ascii **/ {
        }
        replacement.append("*");
    }
    return replacement;
}

std::string Trie::replaceSensitive(const std::string &word) {
    std::set<SensitiveWord> words = getSensitive(word);
    std::string ret;
    int last_index = 0;
    for (auto &item : words) {
        std::string substr = word.substr(item.startIndex, item.len);
        std::string replacement = chinese_or_english_append(substr);

        // 原始内容
        ret.append(word.substr(last_index, item.startIndex - last_index));

        // 替换内容
        ret.append(replacement);
        last_index = item.startIndex + item.len;
    }

    // append reset
    ret.append(word.substr(last_index, word.length() - last_index));

    return ret;
}

void Trie::loadFromFile(const std::string &file_name) {
    std::ifstream ifs(file_name, std::ios_base::in);
    std::string str;
    int count = 0;
    while (getline(ifs, str)) {
        // 转换成小写
        transform(str.begin(), str.end(), str.begin(), ::tolower);
        insert(str);
        count++;
    }
    std::cout << "load " << count << " words" << std::endl;
}

void Trie::loadStopWord(const std::string &file_name) {
    std::ifstream ifs(file_name, std::ios_base::in);
    std::string str;
    int count = 0;
    while (getline(ifs, str)) {
        if (str.length() == 1) {
            stop_words_.insert(str[0]);
            count++;
        } else if (str.empty()) {
            stop_words_.insert(' ');
            count++;
        }
    }
    std::cout << "load " << count << " stop words" << std::endl;
}

#ifdef UNIT_TEST
// performance
void test_time(Trie &t) {
  auto t1 = std::chrono::steady_clock::now();

  std::cout << "你好，加一下微信18301231231:";
  for (auto &&i : t.getSensitive("你好，加一下微信VX18301231231")) {
    std::cout << i.word;
  }
  std::cout << std::endl;

  // run code
  auto t2 = std::chrono::steady_clock::now();
  //毫秒级
  double dr_ms = std::chrono::duration<double, std::milli>(t2 - t1).count();
  std::cout << "耗时: " << dr_ms << std::endl;
}

void func(Trie &t) {
  std::cout << "敏感词为：";
  std::string origin = "你你你你是傻逼啊你，说你呢，你个大笨蛋。";
  char dest[128] = {0};
  origin.copy(dest, origin.length(), 0);

  for (auto &&i : t.getSensitive(origin)) {
    std::cout << "[index=" << i.startIndex << ",len=" << i.len
              << ",word=" << i.word << "],";

    // PS：对于中文敏感词，因为一般汉字占3个字节（也有4个字节），所以这里“*”是正常的3倍。
    // 项目里需要是判断是包含，而不是替换。
    // 所以，如果有这种需求，建议改成wstring和wchat_t来实现
    for (int j = i.startIndex; j < (i.startIndex + i.len); ++j) {
      dest[j] = '*';
    }
  }

  std::cout << "  替换后：" << dest << std::endl;
  std::cout << std::endl;
}

// thread safe
void test_concurrent(Trie &t) {
  for (int i = 0; i < 20; i++) {
    std::thread thd([&]() { func(t); });
    thd.join();
  }

  std::this_thread::sleep_for(std::chrono::microseconds(3000));
}

int testTrie() {
    Trie t;
    t.insert("apple");
    assert(t.search("apple"));            // 返回 true
    assert(t.search("app") == false);     // 返回 false
    assert(t.startsWith("app"));          // 返回 true
    t.insert("app");

    assert(t.search("app"));            // 返回 true
    assert(t.search("this is apple"));  // 返回 true

    t.insert("微信");
    t.insert("vx");
    std::string origin = "你好，请加微信183023102312";
    assert(t.replaceSensitive(origin) == "你好，请加**183023102312");

    t.insert("你是傻逼");
    t.insert("你是傻逼啊");
    t.insert("你是坏蛋");
    t.insert("你个大笨蛋");
    t.insert("我去年买了个表");
    t.insert("shit");

    origin = "SHit，你你你你是傻逼啊你，说你呢，你个大笨蛋。";
    assert(t.replaceSensitive(origin) == "****，你你你****啊你，说你呢，*****。");

    test_time(t);
    test_concurrent(t);

    // 测试汉字的字节数
    // std::string str = "你好啊";
    // std::string test = std::to_wstring(str);

    // for (int i = 0; i < test.length(); i++) {
    //   std::cout << test[i] << std::endl;
    // }

  return 0;
}
#endif // UNIT_TEST