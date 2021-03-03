#include <iostream>

#include <cstring>
#include <cerrno>
#include <netinet/in.h> // ipv4: PF_INET,sockaddr_in ,v6:PF_INET6,sockaddr_in6
#include <sys/socket.h> // socket,bind,listen,accept
#include <unistd.h>     // read,close

const int kSocketError = -1;

int main() {
    int listenFd = ::socket(PF_INET, SOCK_STREAM, 0);
    if (listenFd == kSocketError) {
        std::cout << "create socket error:" << errno << std::endl;
        return 0;
    }
    std::cout << "create socket" << std::endl;

    struct sockaddr addr{};
    int ret = ::bind(listenFd, &addr, sizeof(addr));
    if (ret == kSocketError) {
        std::cout << "bind socket error:" << errno << std::endl;
        return 0;
    }

    std::cout << "bind success" << std::endl;

    ret = ::listen(listenFd, SOMAXCONN);
    if (ret == kSocketError) {
        std::cout << "listen error:" << errno << std::endl;
        return 0;
    }

    struct sockaddr_in peerAddr{};
    socklen_t sockLen = sizeof(sockaddr_in);
    int fd = ::accept(listenFd, (sockaddr *) &peerAddr, &sockLen);
    if (fd == kSocketError) {
        return 0;
    }

    while (true) {
        char buffer[1024] = {};
        ssize_t len = recv(fd, buffer, sizeof(buffer), 0); // wait
        if (len == kSocketError) {
            std::cout << "recv error:" << errno << std::endl;
            break;
        } else {
            std::cout << "recv: " << buffer << std::endl;
            // echo
            len = send(fd, buffer, len, 0);
            if (len == kSocketError) {
                std::cout << "send error:" << errno << std::endl;
                break;
            }
        }
    }

    if (close(listenFd) == kSocketError) {
        std::cout << "close error:" << errno << std::endl;
    } else {
        std::cout << "close success" << std::endl;
    }

    return 0;
}
