/** @file main.h
  * @brief 实现一个单例，需要考虑线程安全的问题
  * 题目：
  * 设计一个类，我们只能生成该类的一个实例。
  *
  * 不好的解法：
  * 1.只适用于单线程环境
  * 2.加锁能在多线程环境下工作，但是效率不高
  *
  * 可行的解法：
  * 加同步锁前后2次判断实例是否已存在。
  *
  * 强推推荐：
  * 1.静态变量只初始化一次的特性。为什么？通过静态变量后面的一个32位内存位来做记录，以标识这个静态变量是否已经初始化
  * 2.静态全局变量初始化
  *
  * @author yingchun.xu
  * @date 2020/7/22
  */

#include <iostream>
#include <thread>

using namespace std;

class CManager {
public:
    static CManager &GetInstance();

    static CManager *GetInstance2();


    void print() {
        printf("CManager address:%p \n", this);
    }

private:
    CManager() {
        cout << "construct" << endl;
    }

    ~CManager() {}

    CManager(const CManager &m) {}

    const CManager &operator=(const CManager &m) { return *this; }

    static CManager *singleton_;
};

/*
 * 第一种方式：静态变量
 */
CManager &CManager::GetInstance() {
    // 局部静态特性的方式实现单实例
    static CManager manager;
    return manager;
}

/**
 * 第二种方式：全局初始化
 */
CManager *CManager::singleton_ = new CManager();

CManager *CManager::GetInstance2() {
    return singleton_;
}

void test1() {
    for (int i = 0; i < 10; i++) {
        auto proc = []() {
//            CManager &m = CManager::GetInstance();
//            m.print();

            CManager::GetInstance2()->print();
        };

        thread t(proc);
        t.detach();
    }
}

int main() {
    /**
     * 第三种方式：直接在main函数里面调用一次GetInstance()初始化
     */

    test1();

    return 0;
}