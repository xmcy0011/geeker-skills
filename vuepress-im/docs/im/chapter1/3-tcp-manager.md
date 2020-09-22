<!-- TOC -->

- [如何管理大量TCP连接](#如何管理大量tcp连接)
    - [经典问题：C10K和C10M](#经典问题c10k和c10m)
    - [理论基础](#理论基础)
        - [Linux下的IO模型](#linux下的io模型)
            - [什么是I/O](#什么是io)
            - [5种I/O模型](#5种io模型)
                - [阻塞式I/O](#阻塞式io)
                - [非阻塞式I/O](#非阻塞式io)
                - [I/O复用模型](#io复用模型)
                - [信号驱动式I/O（不常用）](#信号驱动式io不常用)
                - [异步I/O模型（不常用）](#异步io模型不常用)
                - [5种模型对比](#5种模型对比)
        - [高性能实现：I/O复用](#高性能实现io复用)
            - [三种I/O复用模型](#三种io复用模型)
                - [select](#select)
                - [poll](#poll)
                - [epoll](#epoll)
            - [2种设计模式](#2种设计模式)
                - [reactor(unix)](#reactorunix)
                - [preactor(windows)](#preactorwindows)
        - [延伸阅读](#延伸阅读)
            - [关于共享内存](#关于共享内存)
            - [关于用户态TCP](#关于用户态tcp)
            - [windows下的I/O复用：IOCP](#windows下的io复用iocp)
    - [实现](#实现)
        - [数据模型](#数据模型)
            - [connect, user模型](#connect-user模型)
        - [C++实现](#c实现)
        - [Go实现](#go实现)
        - [Java实现](#java实现)
    - [改进](#改进)
        - [万级别的架构：单机](#万级别的架构单机)
        - [十万级别的架构：多机](#十万级别的架构多机)
        - [百万级别的架构：集群](#百万级别的架构集群)
        - [万到百万的挑战](#万到百万的挑战)
            - [三高：高性能、高可用、高并发](#三高高性能高可用高并发)
            - [高可用技术](#高可用技术)
            - [高并发技术](#高并发技术)
            - [连接"桶"](#连接桶)
            - [分布式集群技术](#分布式集群技术)

<!-- /TOC -->

# 如何管理大量TCP连接

本章的目标是介绍如何管理大量的TCP连接（UDP要用好真不简单，功力有限，抛开不谈），思路如下：

- 先说挑战，从几个经典问题看起。
- 再说理论，看Linux操作系统几种经典的I/O模型是如何解决这些问题的
- 然后说下代码实现
- 最后会深入探讨下万、十万和百万级别连接管理的架构

## 经典问题：C10K和C10M

### 引言

为什么要讨论这2个问题？我印象中小学（2003年）时代一个月才有1次的计算机打字课（进去还要换拖鞋，也是很奇怪），是用的大头机和windows98系统：

![大头机](../images/pc_1990.jpg)

到初中年代（2008），县城里的网吧也还有许多电脑是这种大头机（辐射非常大，玩一会头晕眼花），然后慢慢的才有液晶显示器，直到大学才有自己的第一款笔记本（2010）。

也是从2010年后，自己各种折腾电脑，重装系统，到自己工作，才开始关注CPU，i3、i5和i7，甚至最近2020年新款的mac pro的i9处理器，从单核到双核到4核8线程等等。

计算机飞速发展，现在一台普通的笔记本甚至智能手机都能媲美20世纪的服务器了。【PK图，后续添加】

大学时代，我在i3配置的笔记本上用C#实现了一个windows tcp server，一开始只能支持几百个连接（客户端启动和退出一频繁，服务就崩溃），然后使用了IOCP的技术，实现了几千个连接的支持（受限于笔记本）。直到工作，任意找一台台式机，用Linux写一个epoll服务端，就可以实现上万的连接支持，所以现在C10K（单机支持10*1000个连接）已经不在是问题了。

### C10K

### C10M

## 理论基础

### Linux下的IO模型

```bash
$ wget https://src.fedoraproject.org/repo/pkgs/man-pages-zh-CN/manpages-zh-1.5.1.tar.gz/13275fd039de8788b15151c896150bc4/manpages-zh-1.5.1.tar.gz    # 下载
$ tar zxf manpages-zh-1.5.1.tar.gz -C /usr/src
$ cd /usr/src/manpages-zh-1.5.1/
$ ./configure --disable-zhtw  --prefix=/usr/local/zhman && make && make install
$ echo "alias cman='man -M /usr/local/zhman/share/man/zh_CN' " >>.bash_profile
$ source .bash_profile 
$ cman ls                   # 中文版使用cman，测试一下
```

#### 什么是I/O

首先什么是I/O？

I/O(input/output)，即输入输出设备，现实中键盘和鼠标是输入设备，显示器是输出设备，在《深入理解计算机系统》第一章1.7.4节中，有说到：**文件是对I/O设备的抽象表示，文件就是字节序列，仅此而已。每个I/O设备，包括磁盘、键盘、显示器，甚至网络，都可以看出是文件**。

所以，不难理解在Linux的API中，为什么发送TCP数据包可以调用write()，接收数据包可以调用read()了，在《Linux-UNIX系统编程手册》中第56.5.4节流socket I/O中有描述：

![socket-io](../images/socket-io.jpg)

代码大概是这样：

```c++
while(true){
	// ...
	// 调用read等待网络数据的到来，期间一直阻塞
	size_t len = read(socket_fd, buffer, kMaxBufferLen);
	// 调用write把要发送的数据写入到socket缓冲区，等待发送
	write(socket_fd, buffer, len);
	// ...
}
```


既然网络也被抽象为文件，那么如何使读写（收发）比较快，拥有很高的性能就很关键了，《UNIX网络编程卷1》第6.2节 I/O模型里面介绍了5种模型，我们来一起看看。

#### 5种I/O模型

- 阻塞式I/O
- 非阻塞式I/O
- I/O复用（select和poll）
- 信号驱动式I/O（SIGIO）
- 异步I/O（POSIX的aio_系列函数）

后面2种不常用，主要看前3种即可。



##### 阻塞式I/O

![阻塞式I/O](../images/io-model-blocking.png)

TODO...



##### 非阻塞式I/O

![非阻塞式I/O](../images/io-model-noblocking.png)

TODO...



##### I/O复用模型

![I/O复用模型](../images/io-model-multiplexing.png)

TODO...



##### 信号驱动式I/O（不常用）

![信号驱动式I/O](../images/io-model-signal-driven.png)

TODO...



##### 异步I/O模型（不常用）

![异步I/O模型](../images/io-model-async.png)

TODO...



##### 5种模型对比

![5种模型对比](../images/io-model-compare.png)

TODO...



### 高性能实现：I/O复用

其实最重要的就是这一节吧，实现大量TCP的连接的答案就是epoll技术，但是并不是每种场合下epoll都适用：

- 比如epoll是linux特有的，windows有iocp（完成端口）的技术，所以如果是windows服务器，就不适合
- epoll适合连接数多，但是都不活跃的场景，比如IM（用户虽然在线，但并不是时时刻刻都在发消息）、消息推送等。select/poll适合对吞吐量要求高，连接数少（千级别）的场景，比如音视频传输（每一秒都在传输大量的数据）、文件传输等，当然对于这些对速度有要求的实时传输业务，可能更好的选择是UDP协议。


TODO...

#### 三种I/O复用模型

##### select

##### poll

##### epoll


#### 2种设计模式

##### reactor(unix)
##### preactor(windows)

### 延伸阅读
#### 关于共享内存
#### 关于用户态TCP
#### windows下的I/O复用：IOCP

## 实现

### 数据模型
#### connect, user模型

### C++实现
### Go实现
### Java实现

## 改进

### 万级别的架构：单机

### 十万级别的架构：多机

### 百万级别的架构：集群

### 万到百万的挑战

#### 三高：高性能、高可用、高并发

#### 高可用技术

#### 高并发技术

- 并发编程
- 协程
- 无锁编程
- 缓存技术
- ……

#### 连接"桶"

#### 分布式集群技术