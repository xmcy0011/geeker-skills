/*
 * @Description:
 * @Author: yingchun.xu
 * @Date: 2020-09-04 13:59:26
 * @LastEditTime: 2020-09-07 14:10:45
 * @LastEditors: yingchun.xu
 * @Reference:
 */
#include "trie.h"

const char kEndFlag = '&';

TrieNode::TrieNode() {}

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

void Trie::insert(std::string word) {
    TrieNode *curNode = root_;
    for (int i = 0; i < word.length(); i++) {
        uint32_t code = word[i];
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

/** Returns if the word is in the trie. */
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

/** Returns if there is any word in the trie that starts with the given prefix.
 */
bool Trie::startsWith(std::string prefix) {
    TrieNode *curNode = root_;
    for (int i = 0; i < prefix.length(); i++) {
        curNode = curNode->getSubNode(prefix[i]);
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

std::string Trie::replaceSensitive(const std::string &word) {
    std::set<SensitiveWord> words = getSensitive(word);
    std::string ret = word;
    for (auto &item : words) {
        std::string replacement;
        for (int i = 0; i < item.len; i++) {
            replacement.append("*");
        }
        ret = ret.replace(item.startIndex, item.len, replacement);
    }
    return ret;
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

int main() {
  Trie t;
  // t.insert("微信");
  // t.insert("VX");
  // t.insert("vx");
  t.insert("你是傻逼");
  t.insert("你是傻逼啊");
  t.insert("你是坏蛋");
  t.insert("你个大笨蛋");
  t.insert("我去年买了个表");
  t.insert("shit");

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
#endif