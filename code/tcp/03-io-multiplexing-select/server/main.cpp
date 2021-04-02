#include <iostream>

#include <cstring>
#include <cerrno>
#include <thread>       // thread
#include <netinet/in.h> // ipv4: PF_INET,sockaddr_in ,v6:PF_INET6,sockaddr_in6
#include <sys/socket.h> // socket(),bind(),listen(),accept()
#include <unistd.h>     // read(),close()
#include <arpa/inet.h>  // inet_addr()
#include <fcntl.h>      // fcntl()

#include <sys/select.h> // select()

#include <vector>  // vector

const int kSocketError = -1;

/** @fn main
  * @brief 演示socket的基础调用demo，使用了select I/O复用模型，主要包括select、
  * FD_CLR()、FD_ISSET()、FD_SET()、FD_ZERO()等几个函数。
  * 优点：
  *     1. 相比多线程方案，这里所有的处理都在1个线程上，不用考虑线程并发带来的死锁和同步等问题，内存占用更低，开发更简单。
  *     2. 相比epoll和poll，移植性更好，在windows\mac等平台都支持。
  * 缺点：
  *     1. o(n)级别顺序扫描，效率较低；
  *     2. select最大只支持1024个连接，而poll没有这个限制，但是o(n)级别的扫描，链接数越多，性能则越低。
  *
  * @return
  */
int main() {
    int listenFd = 0;
    int ret = 0;
    int yesReuseAddr = 1;
    struct sockaddr_in addr{};
    std::vector<int> connections; // 已建立的连接
    struct timeval tv{};                // select超时
    fd_set readFds;                     // select 可读fd集合
    int maxFd = 0;                      // select 最大的fd
    char recvBuffer[1024] = {0};        // 接收缓冲区

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
    ret = ::listen(listenFd, SOMAXCONN);
    if (ret == kSocketError) {
        std::cout << "listen error:" << errno << std::endl;
        return 0;
    }

    for (;;) {
        tv.tv_sec = 30;
        tv.tv_usec = 0;

        // select函数会清空各个集合，每次都需要加入
        FD_ZERO(&readFds);
        // Listen的socket加入监听
        FD_SET(listenFd, &readFds);
        maxFd = listenFd;
        // 已连接的socket加入监听
        for (const auto &sockFd : connections) {
            FD_SET(sockFd, &readFds);
            if (sockFd > maxFd) {
                maxFd = sockFd;
            }
        }

        ret = ::select(maxFd + 1, &readFds, nullptr, nullptr, &tv);
        if (ret == -1) {
            std::cout << "select error:" << errno << std::endl;
            continue;
        } else if (ret == 0) {
            std::cout << "select timeout:" << errno << std::endl;
            continue;
        }

        if (FD_ISSET(listenFd, &readFds)) {
            int clientFd = ::accept(listenFd, nullptr, nullptr);
            if (clientFd == kSocketError) {
                std::cout << "accept error:" << clientFd << std::endl;
                continue;
            }
            std::cout << "new connect coming" << std::endl;
            connections.push_back(clientFd);
            if (clientFd > maxFd) { // update
                std::cout << "update maxSock ,before:" << maxFd << ",now:" << clientFd << std::endl;
                maxFd = clientFd;
            }
            FD_SET(clientFd, &readFds); // 加入到监听列表
        } else {
            // select的缺点，每一次都需要o(n)级别的遍历
            for (auto it = connections.begin(); it != connections.end();) {
                int sockFd = *it;
                if (!FD_ISSET(sockFd, &readFds)) { // 没有数据，直接下一个
                    ++it;
                    continue;
                }

                // 获取客户端IP
                struct sockaddr_in peer{};
                socklen_t len = sizeof(sockaddr_in);
                if (::getpeername(sockFd, (sockaddr *) &peer, &len) == kSocketError) {
                    std::cout << "getpeername error" << errno << std::endl;
                    ++it;
                    continue;
                }

                // 因为上面通过FD_ISSET()检查了有数据，所以不会阻塞。
                // 但通常情况下，应该在accept()返回sockfd之后，行设置O_NONBLOCK显示声明为非阻塞I
                ret = ::recv(sockFd, recvBuffer, sizeof(recvBuffer), 0);
                if (ret == 0) { // EOF
                    std::cout << "client " << sockFd << " close ths connection, " << inet_ntoa(peer.sin_addr) << ":"
                              << ::ntohs(peer.sin_port) << std::endl;
                    FD_CLR(sockFd, &readFds);
                    ::close(sockFd);
                    it = connections.erase(it);

                } else if (ret == -1) {
                    std::cout << "recv error:" << errno << std::endl;
                    FD_CLR(sockFd, &readFds);
                    ::close(sockFd);
                    it = connections.erase(it);

                } else {
                    std::cout << "recv from " << inet_ntoa(peer.sin_addr) << ":" << ::ntohs(peer.sin_port)
                              << ": " << recvBuffer << ",len=" << ret << std::endl;
                    // echo
                    ::send(sockFd, recvBuffer, ret, 0);
                    ++it;
                }
            }
        }
    }

    return 0;
}