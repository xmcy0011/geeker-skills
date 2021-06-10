/*
 * @Description:
 * @Author: yingchun.xu
 * @Date: 2020-09-04 13:51:39
 * @LastEditTime: 2020-09-07 14:08:49
 * @LastEditors: yingchun.xu
 * @Reference:
 */
#include "trie.h"
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <locale>
#include <fstream>

double get_time_diff(std::chrono::steady_clock::time_point t1) {
    // run code
    auto t2 = std::chrono::steady_clock::now();
    // 毫秒级
    double dr_ms = std::chrono::duration<double, std::milli>(t2 - t1).count();
    return dr_ms;
}

void test_time(Trie &t, std::string &origin_word) {
    auto t1 = std::chrono::steady_clock::now();
    std::cout << origin_word << std::endl;

    auto r = t.getSensitive(origin_word);

    for (auto &&i : r) {
        std::cout << "[index=" << i.startIndex << ",len=" << i.len
                  << ",word=" << i.word.c_str() << "]," << std::endl;
    }

    // run code
    auto t2 = std::chrono::steady_clock::now();
    // 毫秒级
    double dr_ms = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "cost: " << dr_ms << " ms" << std::endl;

    std::cout << t.replaceSensitive(origin_word) << std::endl << std::endl;
}

// thread safe
void test_concurrent(Trie &t, std::string &origin_word) {
    for (int i = 0; i < 20; i++) {
        std::thread thd([&]() {
            std::cout << "thread: " << t.replaceSensitive(origin_word) << std::endl;
        });
        thd.join();
    }

    std::this_thread::sleep_for(std::chrono::microseconds(3000));
}

void test_time_by_find(std::vector<std::string> &words, std::string &origin_word) {
    std::cout << "find by std::string::find()" << std::endl;
    // 转换成小写
    transform(origin_word.begin(), origin_word.end(), origin_word.begin(), ::tolower);

    // 开始计时
    auto t1 = std::chrono::steady_clock::now();
    for (auto &&i : words) {
        size_t n = origin_word.find(i);
        if (n != std::string::npos) {
            std::cout << "[index=" << n << ",len=" << i.length() << ",word=" << i
                      << "]," << std::endl;
        }
    }

    // run code
    auto t2 = std::chrono::steady_clock::now();
    //毫秒级
    double dr_ms = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "cost : " << dr_ms << " ms" << std::endl << std::endl;
}

void testStopWords() {
    Trie t;
    t.insert("vx");
    t.insert("微信");
    t.loadStopWord("stopwd.txt");

    std::string origin = "请加微-信，v x 号为 12305234";
    auto r = t.getSensitive(origin);

    for (auto &&i : r) {
        std::cout << "[index=" << i.startIndex << ",len=" << i.len
                  << ",word=" << i.word.c_str() << "]," << std::endl;
    }

    std::cout << t.replaceSensitive(origin) << std::endl;
}

int main() {
    // trie: small
    Trie trie2;
    trie2.insert("你是傻逼");
    trie2.insert("你是傻逼啊");
    trie2.insert("你是坏蛋");
    trie2.insert("你个大笨蛋");
    trie2.insert("我去年买了个表");
    trie2.insert("qq"); // 都会转换为小写
    trie2.insert("vx");
    std::string origin = "SHit，QQ,VX 你你你你是傻逼啊你，说你呢，你个大笨蛋。";

    test_time(trie2, origin);

    // KMP: small
    std::vector<std::string> words;
    words.emplace_back("你是傻逼");
    words.emplace_back("你是傻逼啊");
    words.emplace_back("你是坏蛋");
    words.emplace_back("你个大笨蛋");
    words.emplace_back("我去年买了个表");
    words.emplace_back("shit");
    test_time_by_find(words, origin);

    // trie: large from file
    Trie t;
    t.loadFromFile("word.txt");

    origin = "你个傻逼，小姐姐还不赶紧加VX，微信，扣扣是Qq3306 4343，你奶奶的。。。赶快加";

    auto t1 = std::chrono::steady_clock::now();
    std::cout << "origin: " << origin << std::endl
              << "filter: " << t.replaceSensitive(origin) << std::endl
              << "cost: " << get_time_diff(t1) << " ms" << std::endl << std::endl;

    // KMP: large from file
    std::ifstream ifs("word.txt", std::ios_base::in);
    std::string str;
    words.clear(); // clear
    while (getline(ifs, str)) {
        words.emplace_back(str);
    }
    test_time_by_find(words, origin);

    // thread safe
    test_concurrent(trie2, origin);

    // stop words
    std::cout << "test stop words" << std::endl;
    testStopWords();
    return 0;
}