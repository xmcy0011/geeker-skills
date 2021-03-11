#include <iostream>

#include <cerrno>
#include <netinet/in.h> // ipv4: PF_INET,sockaddr_in ,v6:PF_INET6,sockaddr_in6
#include <sys/socket.h> // socket(),bind(),listen(),accept()
#include <unistd.h>     // read(),close()
#include <arpa/inet.h>  // inet_addr()
#include <fcntl.h>      // fcntl()

#include <sys/epoll.h>  // epoll
#include <vector>       // vector

const int kSocketError = -1;

#define USE_ET  // 启用边缘触发

void onNewConnect(const int &epFd, const int &listenFd) {
    int clientFd = ::accept(listenFd, nullptr, nullptr);
    if (clientFd == kSocketError) {
        std::cout << "accept error:" << clientFd << std::endl;
        return;
    }

    // 加入到兴趣列表
    struct epoll_event ev{};
    ev.events = EPOLLIN;

    // 边缘触发模式，追加标志。使用'或'(|)拼接
#ifdef USE_ET
    ev.events |= EPOLLET;
    // 边缘触发模式，只支持非阻塞IO non-block
    int ret = ::fcntl(clientFd, F_SETFL, O_NONBLOCK);
    if (ret == kSocketError) {
        std::cout << "fcntl error:" << errno << std::endl;
        return;
    }
#endif

    ev.data.fd = clientFd;
    if (::epoll_ctl(epFd, EPOLL_CTL_ADD, clientFd, &ev) == kSocketError) {
        std::cout << "epoll_ctl error:" << errno << std::endl;
        ::close(clientFd);
    }

    if (ev.events & EPOLLET) {
        std::cout << "new connect coming,set EPOLLET" << std::endl;
    } else {
        std::cout << "new connect coming" << std::endl;
    }
}

#ifdef USE_ET

void onRead(const int &epFd, const int &fd, const int &listenFd, char recvBuffer[], int bufferLen) {
    // 获取客户端IP
    struct sockaddr_in peer{};
    socklen_t len = sizeof(sockaddr_in);
    if (::getpeername(fd, (sockaddr *) &peer, &len) == kSocketError) {
        std::cout << "getpeername error" << errno << std::endl;
        return;
    }

    // 边缘触发模式下，要一次性从socket的recv缓冲区中把数据读出来，
    // 否则下一次不再触发读事件，导致数据丢失
    // 注意：这里就会导致所谓的TCP粘包问题，应用层协议自己通过TLV处理，这里只是为了方便演示故简化
    char *recvBufferStart = recvBuffer;
    int ret = 0;
    int recvTotalLen = 0;
    while (true) {
        ret = ::recv(fd, recvBufferStart, bufferLen - recvTotalLen, 0);
        if (ret == 0) { // EOF，0代表对端断开连接，需要关闭socket
            std::cout << "EOF: client " << fd << " close ths connection, " << inet_ntoa(peer.sin_addr) << ":"
                      << ::ntohs(peer.sin_port) << std::endl;
            // 移除
            if (::epoll_ctl(epFd, EPOLL_CTL_DEL, fd, nullptr) == kSocketError) {
                std::cout << "epoll_ctl error:" << errno << std::endl;
            }
            ::close(fd);
            break;
        } else if (ret < 0) {
            break;
        }
        recvTotalLen += ret;
        recvBufferStart += ret;
    }

    if (ret == -1 && recvTotalLen == 0) {
        std::cout << "recv error:" << errno << std::endl;
        if (::epoll_ctl(epFd, EPOLL_CTL_DEL, fd, nullptr) == kSocketError) {
            std::cout << "epoll_ctl error:" << errno << std::endl;
        }
        ::close(fd);

    } else {
        std::cout << "recv from " << inet_ntoa(peer.sin_addr) << ":" << ::ntohs(peer.sin_port)
                  << ": " << std::string(recvBuffer, recvTotalLen) << ",len=" << recvTotalLen << std::endl;
        // echo
        ::send(fd, recvBuffer, recvTotalLen, 0);
    }
}

#elif
void onRead(const int &epFd, const int &fd, const int &listenFd, char recvBuffer[], int bufferLen) {
    // 获取客户端IP
    struct sockaddr_in peer{};
    socklen_t len = sizeof(sockaddr_in);
    if (::getpeername(fd, (sockaddr *) &peer, &len) == kSocketError) {
        std::cout << "getpeername error" << errno << std::endl;
        return;
    }

    int ret = ::recv(fd, recvBuffer, bufferLen, 0);
    if (ret == 0) { // EOF
        std::cout << "EOF: client " << fd << " close ths connection, " << inet_ntoa(peer.sin_addr) << ":"
                  << ::ntohs(peer.sin_port) << std::endl;
        // 移除
        if (::epoll_ctl(epFd, EPOLL_CTL_DEL, fd, nullptr) == kSocketError) {
            std::cout << "epoll_ctl error:" << errno << std::endl;
        }
        ::close(fd);

    } else if (ret == -1) {
        std::cout << "recv error:" << errno << std::endl;
        if (::epoll_ctl(epFd, EPOLL_CTL_DEL, fd, nullptr) == kSocketError) {
            std::cout << "epoll_ctl error:" << errno << std::endl;
        }
        ::close(fd);

    } else {
        std::cout << "recv from " << inet_ntoa(peer.sin_addr) << ":" << ::ntohs(peer.sin_port)
                  << ": " << recvBuffer << ",len=" << ret << std::endl;
        // echo
        ::send(fd, recvBuffer, ret, 0);
    }
}
#endif

void onClose(const int &epFd, const int &fd) {
    std::cout << "remove close connection" << std::endl;

    if (::epoll_ctl(epFd, EPOLL_CTL_DEL, fd, nullptr) == kSocketError) {
        std::cout << "epoll_ctl error:" << errno << std::endl;
    }
}

/** @fn main
  * @brief 演示socket的基础调用demo，使用了默认同步I/O阻塞+多线程的方式。
  * 优点：即对上一个进行了改进，能同时支持多个连接
  * 缺点：线程启动代价比较大且较占用内存，不过NGINX早期好像是使用了这种方式，实现了高并发，但是现在已经过时，不推荐使用。
  *
  * @return
  */
int main() {
    int listenFd = 0;
    int ret = 0;
    int yesReuseAddr = 1;
    struct sockaddr_in addr{};
    int epFd = 0;                       // epoll实例
    int maxFiles = 1024;                // 最大可打开文件的数量
    int fdNum = 0;                      // epoll_wait返回产生事件的描述法数量
#ifdef USE_ET
    char recvBuffer[10 * 1024] = {0};   // 接收缓冲区，边缘触发模式下要一次性读完，所以缓冲区要大一些
#elif
    char recvBuffer[1024] = {0};        // 接收缓冲区
#endif

    // get max open file
    maxFiles = static_cast<int>(::sysconf(_SC_OPEN_MAX));
    if (maxFiles == kSocketError) {
        return 0;
    }
    std::cout << maxFiles << " max file can open" << std::endl;

    // create socket
    listenFd = ::socket(PF_INET, SOCK_STREAM, 0);
    if (listenFd == kSocketError) {
        std::cout << "create socket error:" << errno << std::endl;
        return 0;
    }
    std::cout << "create socket" << std::endl;

    // non-block
    ret = ::fcntl(listenFd, F_SETFL, O_NONBLOCK);
    if (ret == kSocketError) {
        std::cout << "fcntl error:" << errno << std::endl;
        return 0;
    }

    // SO_REUSEADDR
    if (::setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &yesReuseAddr, sizeof(yesReuseAddr)) == kSocketError) {
        std::cout << "setsockopt error:" << errno << std::endl;
        return 0;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8088);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ret = ::bind(listenFd, (sockaddr *) &addr, sizeof(addr));
    if (ret == kSocketError) {
        std::cout << "bind socket error:" << errno << std::endl;
        return 0;
    }

    std::cout << "bind success,start listen..." << std::endl;
    // 标识文件描述符为被动socket
    ret = ::listen(listenFd, SOMAXCONN);//SOMAXCONN in ubuntu 4096,in centos7,128
    if (ret == kSocketError) {
        std::cout << "listen error:" << errno << std::endl;
        return 0;
    }

    maxFiles /= 2;
    epFd = ::epoll_create(maxFiles); // 只是告诉内核一个大致数目，不是最大数量
    if (epFd == kSocketError) {
        std::cout << "epoll_create error" << errno << std::endl;
        return 0;
    }

    // listen的socket加入到epoll监听列表
    struct epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = listenFd;
    ret = ::epoll_ctl(epFd, EPOLL_CTL_ADD, listenFd, &ev);
    if (ret == kSocketError) {
        std::cout << "epoll_ctl error:" << errno << std::endl;
        return 0;
    }

    struct epoll_event events[maxFiles];
    for (;;) {
        fdNum = ::epoll_wait(epFd, events, maxFiles, -1);
        if (fdNum == -1) {
            std::cout << "epoll_wait error:" << errno << std::endl;
            continue;
        } else if (fdNum == 0) {
            std::cout << "epoll_wait timeout:" << errno << std::endl;
            continue;
        }

        for (int i = 0; i < fdNum; ++i) {
            const epoll_event &e = events[i];
            int fd = e.data.fd;

            if (e.events & EPOLLIN) {
                if (fd == listenFd) {// 监听socket上读事件，代表新的连接到来
                    onNewConnect(epFd, listenFd);
                } else {
                    onRead(epFd, fd, listenFd, recvBuffer, sizeof(recvBuffer));
                }
            }
#ifdef EPOLLRDHUP
            else if (e.events & EPOLLRDHUP) { // linux 2.6.17 以上
                onClose(epFd, fd);
            }
#endif
            else if (e.events & (EPOLLHUP | EPOLLERR | EPOLLPRI)) { // 挂断、错误、收到高优先级数据都需要关闭连接
                onClose(epFd, fd);
            }
        }

    }

    return 0;
}