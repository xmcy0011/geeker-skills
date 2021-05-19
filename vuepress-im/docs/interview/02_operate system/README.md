# 操作系统

## 考察点

- OS 四大模块的理论知识
  - 进程与线程管理
  - 内存管理
  - IO与文件系统
  - 设备管理
- 了解 Linux 内核部分实现原理
  - 内存管理
  - 进程管理
  - 虚拟文件系统等
- 内存
- 进程
- I/O

## 书籍



1. 《现代操作系统》《操作系统—精髓与设计原理》

   偏理论的书，可以了解操作系统的全貌，这部分看完你应该对下面这些话题有一个清晰认知了。

   - 操作系统由哪些构成

   - 进程的状态、切换、调度

   - 进程间通信方式（共享内存、管道、消息）

   - 进程和线程的区别

   - 线程的实现方式（一对一、多对一等）

   - 互斥与同步（信号量、管程、锁）

   - 死锁检测与避免

   - 并发经典的问题：读者写者、哲学家就餐问题

   - 为什么需要虚拟内存，MMU 具体如何做地址转换的

   - 内存为什么分段、分页

   - 页面置换算法

   - 文件系统是如何组织的

   - 虚拟文件系统（VFS）是如何抽象的




2. 《Linux内核设计与实现》

   但是这还不够，看完偏理论的书，当面试官问「进程和线程的区别」时。

   大概只能回答出「进程是资源分配的最小单位，线程是CPU调度的最小单位，balabala...」这样正确却普通的答案。

   但是如果你了解 Linux 内核的实现，就可以实际出发，讲讲 Linux 中进程和线程是如何创建的，区别在哪里。

   比如在 Linux 中进程和线程实际上都是用一个结构体 task_struct来表示一个执行任务的实体。进程创建调用fork 系统调用，而线程创建则是 pthread_create 方法，但是这两个方法最终都会调用到 do_fork 来做具体的创建操作 ，区别就在于传入的参数不同。

   深究下去，你会发现 Linux 实现线程的方式简直太巧妙了，实际上根本没有**线程**，它创建的就是进程，只不过通过参数指定多个进程之间共享某些资源（如虚拟内存、页表、文件描述符等），函数调用栈、寄存器等线程私有数据则独立。

   这样是不是非常符合理论书上的定义：同一进程内的多个线程共享该进程的资源，但线程并不拥有资源，只是使用他们。

   这也算符合 Unix 的哲学了— KISS（Keep It Simple, Stupid）。

   但是在其它提供了专门线程支持的系统中，则会在进程控制块（PCB）中增加一个包含指向该进程所有线程的指针，然后再每个线程中再去包含自己独占的资源。

   这算是非常正统的实现方式了，比如 Windows 就是这样干的。

   但是相比之下 Linux 就显得取巧很多，也很简洁。

   对于进程、线程这块你还可以把 fork、vfork、clone 、pthread_create 这些模块关系彻底搞清楚，对你理解 Linux 下的进程实现有非常大的帮助。

   说了这么多，就是想强调一下理论联系实际的重要性。

   特别是操作系统，最好的实践就是看下 Linux 内核是怎么实现的，当然不是叫你直接去啃 Linux 源码，那不是一般人能掌握的。

   最好的方式是看书，书的脉络给你理得很清晰。

   

   