# 后端开发知识点一览图

## C++

主要看一本书：《C++ Primer 》，以掌握C++语法为主（重点掌握，务必熟悉底层机制原理）

主要包括6个部分：
1. 基础语法
2. 面向对象
3. STL
4. C++11/14
5. 内存管理
6. 设计模式

其中template泛型编程角

### 基础语法

- 指针和引用的区别
- 野指针产生与避免
- static：修饰局部变量、全局变量、类中成员变量、类中成员函数
- const：修饰变量、指针、类对象、类中成员函数
- volatile：避免编译器指令优化
- extern：修饰全局变量
- #define宏
- C和C++区别

### 面向对象

- 理解big three（拷贝构造、赋值构造、析构函数）
- 深拷贝、浅拷贝
- [c++11移动构造函数](http://avdancedu.com/a39d51f9/)
- new/delete和malloc/free
- class和struct，以及区别
- 封装
    - 访问限定符 public、private、protected
- 多态
    - 静态绑定（重写、重载、模板）和动态绑定
    - 纯虚函数(抽象类)和虚函数
    - 虚函数实现机制：虚函数表
    - 继承时，父类的析构函数是否为虚函数？构造函数能不能为虚函数？为什么？
- 继承
    - 虚析构
    - 继承原理、虚继承、菱形继承
    - 重载、重写和隐藏
    - public、private、protected继承

### STL

- 顺序容器：vector（动态数组）、list（链表）、deque（队列）
- 关联容器：map（集合）、set（映射）、c++11的unorder_map和unorder_set
- vector背后原理（动态扩容）、常用操作：增删改查的时间复杂度、emplace_front、emplace_back
- vector与list比较
- [迭代器失效的情况：顺序容器和关联容器](https://blog.csdn.net/qq_37964547/article/details/81160505)
- map/set背后的实现原理：红黑树
- map的操作：增删改查的时间复杂度、at()

### 现代C++（11/14）

- 智能指针原理：引用计数、RAII（资源获取即初始化）思想
- 智能指针使用：shared_ptr、weak_ptr、unique_ptr等
- 左值、[右值](http://avdancedu.com/a39d51f9/)、右值引用、std::move语义、std::forward完美转发等
- 四种类型转换：static_cast, dynamic_cast, const_cast, reinterpret_cast
- 类型推导：auto、decltype
- 基于范围的 for 循环for(auto& i : res){}
- Lambda 表达式（匿名函数）
- [一致性初始化与初值列](https://blog.csdn.net/darkbus111/article/details/78189340)
- nullptr替代 NULL

### 内存管理

- 类的内存布局、struct内存对齐
- 内存分区：栈(Stack Segment) -> 未使用内存(Unused Memory) -> 堆(Heap Segment) -> 全局数据区(Global Data Segment) -> 常量区(Constant Segment) -> 代码区(Code Segment)
- 内存池，《STL源码剖析》allocate 包装 malloc，deallocate包装free
- this指针
- 内存泄漏和检测，BoundsChecker和Valgrind
- 空类大小、空类有哪些函数
- sizeof与strlen对比

### 设计模式

- 常用的设计模式
- 线程安全的单例模式
