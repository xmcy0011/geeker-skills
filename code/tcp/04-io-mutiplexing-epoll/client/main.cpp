#include <iostream>

#include <cerrno>
#include <thread>
#include <sys/socket.h> // bind,connect
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>  // inet_addr()
#include <unistd.h>     // close
#include <functional>

const int kSocketError = -1;

void robot(int id) {
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd == kSocketError) {
        std::cout << "socket error:" << errno << std::endl;
        return;
    }

    struct sockaddr_in serverIp{};
    serverIp.sin_family = AF_INET;
    serverIp.sin_port = htons(8088);
    serverIp.sin_addr.s_addr = inet_addr("127.0.0.1");

    int ret = ::connect(fd, (sockaddr *) &serverIp, sizeof(serverIp));
    if (ret == kSocketError) {
        std::cout << "connect error:" << errno << std::endl;
        return;
    }
    std::cout << id << " connect remote success" << std::endl;

    // recv 5秒超时
    struct timeval timeout = {5, 0};
    // send 超时 这样设置
    //ret = ::setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout, sizeof(timeout));
    ret = ::setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));
    if (ret == kSocketError) {
        std::cout << "setsockopt err" << errno << std::endl;
        return;
    }

    char buffer[1024] = {0};
    char recvBuffer[1024] = {0};
    for (int i = 0; i < 10; ++i) {
        long sleep = ::random() % 3000 + 3000;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        int len = sprintf(buffer, "hello %d", i);

        std::cout << "send " << std::endl;
        ret = ::send(fd, buffer, len, 0);
        if (ret == kSocketError) {
            std::cout << "send error:" << errno << std::endl;
            break;
        }

        ret = ::recv(fd, recvBuffer, sizeof(recvBuffer), 0);
        if (ret == kSocketError) {
            std::cout << "recv error:" << errno << std::endl;
            continue;
        }
        std::cout << "recv from:" << recvBuffer << std::endl;
    }
    ::close(fd);
    std::cout << "close " << id << std::endl;
}

int main() {
    ::srandom(time(nullptr));

    const int clientNum = 1;
    for (int i = 0; i < clientNum; ++i) {
        std::thread t(robot, i);
        t.detach();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // wait client exit
    std::this_thread::sleep_for(std::chrono::seconds(60));
    return 0;
}
