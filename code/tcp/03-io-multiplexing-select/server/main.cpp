#include <iostream>

#include <cstring>
#include <cerrno>
#include <thread>       // thread
#include <netinet/in.h> // ipv4: PF_INET,sockaddr_in ,v6:PF_INET6,sockaddr_in6
#include <sys/socket.h> // socket(),bind(),listen(),accept()
#include <unistd.h>     // read(),close()
#include <arpa/inet.h>  // inet_addr()
#include <fcntl.h>      // fcntl()

const int kSocketError = -1;

/** @fn main
  * @brief 演示socket的基础调用demo，使用了默认同步I/O阻塞+多线程的方式。
  * 优点：即对上一个进行了改进，能同时支持多个连接
  * 缺点：线程启动代价比较大且较占用内存，不过NGINX早期好像是使用了这种方式，实现了高并发，但是现在已经过时，不推荐使用。
  *
  * @return
  */
int main() {
    int listenFd = ::socket(PF_INET, SOCK_STREAM, 0);
    if (listenFd == kSocketError) {
        std::cout << "create socket error:" << errno << std::endl;
        return 0;
    }
    std::cout << "create socket" << std::endl;

    // non-block
    int ret = ::fcntl(listenFd, F_SETFL, O_NONBLOCK);
    if (ret == kSocketError) {
        std::cout << "fcntl error:" << errno << std::endl;
        return 0;
    }

    struct sockaddr_in addr{};
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

    // 死循环，永不退出
    while (true) {
        struct sockaddr_in peerAddr{};
        socklen_t sockLen = sizeof(sockaddr_in);
        // will sleep, until one connection coming
        int fd = ::accept(listenFd, (sockaddr *) &peerAddr, &sockLen);
        if (fd == kSocketError) {
            return 0;
        }

        // 改进：
        auto proc = [fd, peerAddr]() {
            std::cout << "new connect coming,accept..." << std::endl;
            while (true) {
                char buffer[1024] = {};
                ssize_t len = recv(fd, buffer, sizeof(buffer), 0); // wait
                if (len == kSocketError) {
                    std::cout << "recv error:" << errno << std::endl;
                    break;

                } else if (len == 0) {
                    std::cout << "recv error:" << errno << std::endl;
                    break;

                } else {
                    std::cout << "recv" << ::inet_ntoa(peerAddr.sin_addr) << ":" << peerAddr.sin_port
                              << " " << buffer << ",len=" << len << std::endl;
                    // echo
                    len = send(fd, buffer, len, 0);
                    if (len == kSocketError) {
                        std::cout << "send error:" << errno << std::endl;
                        break;
                    }
                }
            }
            ::close(fd);
            std::cout << "remote " << ::inet_ntoa(peerAddr.sin_addr) << "close connection" << std::endl;
        };

        std::thread thread(proc);
        // 分离线程，主线程结束时继续存在；线程结束后，立马回收资源。
        thread.detach();
    }

    return 0;
}