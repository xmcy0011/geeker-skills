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

void test_time(Trie &t) {
    auto t1 = std::chrono::steady_clock::now();
    auto r = t.getSensitive("SHit，你你你你是傻逼啊你，说你呢，你个大笨蛋。");

    for (auto &&i : r) {
        std::cout << "[index=" << i.startIndex << ",len=" << i.len
                  << ",word=" << i.word.c_str() << "],";
    }
    std::cout << std::endl;

    auto word = t.replaceSensitive("SHit，你你你你是傻逼啊你，说你呢，你个大笨蛋。");
    std::cout << word << std::endl;

    // run code
    auto t2 = std::chrono::steady_clock::now();
    // 毫秒级
    double dr_ms = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "耗时（毫秒）: " << dr_ms << std::endl;
}

// thread safe
void test_concurrent(Trie &t) {
    for (int i = 0; i < 20; i++) {
        //std::thread thd([&]() { func(t); });
        //thd.join();
    }

    std::this_thread::sleep_for(std::chrono::microseconds(3000));
}

void test_time_by_find() {
    std::string origin = "SHit，你你你你是傻逼啊你，说你呢，你个大笨蛋。";
    std::vector<std::string> words;
    words.push_back("你是傻逼");
    words.push_back("你是傻逼啊");
    words.push_back("你是坏蛋");
    words.push_back("你个大笨蛋");
    words.push_back("我去年买了个表");
    words.push_back("shit");

    // 开始计时
    auto t1 = std::chrono::steady_clock::now();

    // 转换成小写
    transform(origin.begin(), origin.end(), origin.begin(), ::tolower);

    for (auto &&i : words) {
        size_t n = origin.find(i);
        if (n != std::string::npos) {
            std::cout << "[index=" << n << ",len=" << i.length() << ",word=" << i
                      << "],";
        }
    }
    std::cout << std::endl;

    // run code
    auto t2 = std::chrono::steady_clock::now();
    //毫秒级
    double dr_ms = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "耗时（毫秒）: " << dr_ms << std::endl;
}

int main() {
    Trie t;
    // t.insert("apple");
    // printf("%d \n", t.search("apple"));   // 返回 true
    // printf("%d \n", t.search("app"));     // 返回 false
    // printf("%d \n", t.startsWith("app")); // 返回 true
    // t.insert("app");
    // printf("%d \n", t.search("app"));     // 返回 true
    // printf("%d \n", t.search("this is apple")); // 返回 false，为什么？

    // t.insert("微信");
    // t.insert("vx");
    // std::cout << t.search("你好，请加微信183023102312") << std::endl;

    t.insert("你是傻逼");
    t.insert("你是傻逼啊");
    t.insert("你是坏蛋");
    t.insert("你个大笨蛋");
    t.insert("我去年买了个表");
    t.insert("shit");

    test_time(t);
    test_time_by_find();
    // test_concurrent(t);

    // 测试汉字的字节数
    // std::string str = "你好啊";
    // std::wstring test = std::to_wstring(str);

    // for (int i = 0; i < test.length(); i++) {
    //   std::cout << test[i] << std::endl;
    // }


    return 0;
}