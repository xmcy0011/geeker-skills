# C++

## 类

1. 构造函数有哪几种？无参、有参、拷贝、赋值、移动
2. 什么情况下必须得实现构造函数，否则编译器报错？[C++必须要存在非默认构造函数的条件](https://blog.csdn.net/yueyansheng2/article/details/44459313)
3. 在什么情况下必须实现拷贝构造函数？[什么时候需要定义拷贝构造函数](https://blog.csdn.net/suxinpingtao51/article/details/36890809)
4. 浅拷贝与深拷贝 [c++ 构造函数详解](https://blog.csdn.net/qq_29339467/article/details/90719951)
5. =default或者=delete [C++构造函数的default和delete](https://www.cnblogs.com/xiangtingshen/p/11005281.html)
6. =0，代表纯虚函数 《C++ Primer 第五版 541页》
7. 函数override关键字解决什么问题 《C++ Primer 第五版 538页》
8. 三种继承的区别？默认继承是什么

## 类型转换

1. explicit 构造函数禁止隐式转换
2. constexpr 构造函数必须初始化所有成员
2. [C++11]static_cast、dynamic_cast、const_cast、reinterpret_cast

## 多线程

1. mutable 有什么作用和场景
2. volite的作用

## 函数
1. inline函数原理和作用
> 有点类似宏，会把代码替换过来，这样在很多地方调用这个inline函数，则代码量会变大，程序变大。但是在性能上因为省掉了函数的调用跳转过程（调用函数需要时间和内存），所以更快一些。是否是在代码量不大或者循环体内的函数使用inline？  

> 为什么函数调用需要时间和内存？  
> 《剑指OFFER 第二版 74页》：递归由于是函数调用自身，而函数调用是有时间和空间的消耗的：每一次函数调用，都需要在内存栈中分配空间以保存参数、返回地址及临时变量，而且往栈里压入数据和弹出数据都需要时间。这就不难理解递归实现的效率不如循环了。  

> 递归函数可以用inline实现吗？  
> inline只是建议，具体由编译器决定。实践在g++ -o0情况下作为普通函数被调用，在-o1的时候，递归函数也被展开了。。

2. 函数后面加const有什么作用
3. 函数重载的原理
4. 继承和多态的原理（虚函数）

## STL

### Vector

1. vector的下标和at的区别，size和capacity的区别
2. vector的push_back和emplace_back的区别 [https://blog.csdn.net/p942005405/article/details/84764104]

### 红黑树

1. 为什么STL和linux都使用红黑树作为平衡树的实现？
2. 简单描述一下红黑树的特点，delete和move操作的区别

