/** @file main.h
  * @brief 实现一个函数赋值操作符
  *
  * 题目：
  * 如下类型为CMyString的声明，请为该类型添加赋值运算符函数。
class CMyString {
public:
    CMyString(char *data = nullptr);
    CMyString(const CMyString &str);
    ~CMyString(void);
private:
    char *data_;
};
  *
  * @author yingchun.xu
  * @date 2020/7/22
  */
#include <iostream>

using namespace std;

class CMyString {
public:
    CMyString(char *data = nullptr) : data_(nullptr) {
        data_ = data;
    }

    CMyString(const CMyString &str) : data_(nullptr) {
        data_ = str.data_;
    }

    ~CMyString(void) {
        if (data_ != nullptr) {
            cout << "delete data_:" << data_ << endl;
            delete data_;
        }
    }

    /**
     * 1.能返回自身的引用，可以连续赋值，str1=str2=str3
     * 2.传入的是否为常量引用，避免复制。同时，我们在内部不会修改该状态，还需要使用const修饰
     * 3.是否释放已有的内存
     * 4.判断传入的实例是不是当前的实例，否则释放内存后就会引起错误，导致找不到需要赋值的内容了
     * @param str
     * @return
     */
#if 1

    CMyString &operator=(const CMyString &str) {
        if (this == &str) {
            return *this;
        }

        if (data_ != nullptr) {
            delete[] data_;
            data_ = nullptr;
        }

        data_ = new char[strlen(str.data_) + 1]; // 此时有可能内存不足
        strcpy(data_, str.data_);

        return *this;
    }

#else

    /**
     * 更好的解法，考虑到了异常安全性的情况。
     * 但是实际测试的时候会有异常，这个实践中有疑问
     * @param str
     * @return
     */
    CMyString &operator=(const CMyString &str) {
        if (this != &str) {
            CMyString temp(str.data_);

            char *temp_data = temp.data_;
            temp.data_ = data_; // 借用临时变量退出前调用析构函数，释放之前的内存
            data_ = temp_data;
        }
        return *this;
    }

#endif

private:
    char *data_;
};


void test() {
    char *c1 = new char[1];
    c1[0] = '1';
    char *c2 = new char[1];
    c2[0] = '2';
    //char *c3 = new char[1];
    //c3[0] = '3';

    CMyString str1(c1);
    CMyString str2(c2);
    //CMyString str3(c3);

    str1 = str2;// = str3;
    cout << "operator =" << endl;
}

int main() {
    test();
    return 0;
}