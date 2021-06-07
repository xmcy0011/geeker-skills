/** @file main.h
  * @brief OpenSSL库使用示例，为了兼顾简单和实用，这里拿PerConnectionPerThread模型来演示。即一个连接一个线程
  *        ，注意：实际中为了更高的性能，通常是搭配epoll I/O复用来实现。有单Reactor单线程，单Reactor多线程，主从Reactor多线程几种模型
  * @author teng.qing
  * @date 2021/5/13
  */

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string>
#include <iostream>
#include <chrono>

#include <thread>

/** @fn create_socket
  * @brief 创建一个监听的socket
  * @param [in]listenPort:监听端口
  * @return
  */
int create_socket(int listenPort) {
    int sockFd = 0;
    struct sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(listenPort);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    sockFd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        printf("create socket error:%d", errno);
        exit(EXIT_FAILURE);
    }

    // SO_REUSEADDR
    int yesReuseAddr = 1;
    if (::setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &yesReuseAddr, sizeof(yesReuseAddr)) == -1) {
        std::cout << "setsockopt error:" << errno << std::endl;
        return 0;
    }

    if (bind(sockFd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        std::cout << "Unable to bind:" << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(sockFd, SOMAXCONN) < 0) {
        std::cout << "Unable to listen:" << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    return sockFd;
}

/** @fn init_openssl
  * @brief 全局初始化openssl库，只需要调用一次
  * @return
  */
void init_openssl() {
    SSL_load_error_strings();       // 载入所有SSL 错误消息
    OpenSSL_add_all_algorithms();   // 加载所有支持的算法
}

/** @fn cleanup_openssl
  * @brief 退出前清理openssl
  * @return
  */
void cleanup_openssl() {
    EVP_cleanup();
}

/** @fn create_context
  * @brief 创建一个全局SSL_CTX，存储证书等信息
  * @return
  */
SSL_CTX *create_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    /* 以SSL V2 和 V3 标准兼容方式产生一个SSL_CTX ，即SSL Content Text */
    /* 也可以用SSLv2_server_method() 或SSLv3_server_method() 单独表示V2 或V3 标准*/
    //method = SSLv3_server_method();
    method = SSLv23_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

/** @fn configure_context
  * @brief 设置证书
  *
  * 生成自签名证书命令如下：
  * 1. 生成私钥
  * openssl genrsa -out google.com.key 2048
  *
  * 2. 生成CSR（证书签名请求）
  * openssl req -new -out google.com.csr -key google.com.key
  *

Country Name (2 letter code) [AU]:CN
State or Province Name (full name) [Some-State]:Shanghai
Locality Name (eg, city) []:Shanghai
Organization Name (eg, company) [Internet Widgits Pty Ltd]:Google Ltd
Organizational Unit Name (eg, section) []:google.com
Common Name (e.g. server FQDN or YOUR name) []:*.google.com #这一项必须和你的域名一致
Email Address []:kefu@google.com
A challenge password []:fG!#tRru
An optional company name []:Google.com

  * 3. 生成自签名证书（100年过期）
  * openssl x509 -req -in google.com.csr -out google.com.cer -signkey google.com.key -CAcreateserial -days 36500
  *
  * 4. 生成服务器crt格式证书
  * openssl x509 -inform PEM -in google.com.cer -out google.com.crt
  *
  * 5. 生成PEM公钥
  * openssl x509 -in google.com.crt -outform PEM -out google.com.pem
  *
  * 最后，google.com.pem 和 google.com.key 是本程序需要的 公钥和私钥
  *
  * 附录：
  * 生成IOS客户端p12格式根证书（输入密码fG!#tRru）
  * openssl pkcs12 -export -clcerts -in google.com.cer -inkey google.com.key -out google.com.p12
  *
  * 生成Android客户端bks格式证书
  * 略
  *
  * @param [in]ctx: SSL上下文
  * @param [in]certPath: 证书文件
  * @param [in]privateKeyPath: 私钥文件
  * @return
  */
void configure_context(SSL_CTX *ctx, std::string certPath, std::string privateKeyPath) {
    SSL_CTX_set_ecdh_auto(ctx, 1);

    // 载入用户的数字证书， 此证书用来发送给客户端。证书里包含有公钥
    if (SSL_CTX_use_certificate_file(ctx, certPath.c_str() /*"cert.pem"*/, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // 载入用户私钥
    if (SSL_CTX_use_PrivateKey_file(ctx, privateKeyPath.c_str()/*"key.pem"*/, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // 检查用户私钥是否正确
    if (!SSL_CTX_check_private_key(ctx)) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
}

/** @fn opensslErrorCheck
  * @brief opensslErrorCheck
  * @param [in]ssl: SSL实例
  * @param [in]retCode: SSL_read/SSL_write返回值
  * @param [in]isError: 是否确实发生了错误
  * @return
  */
void opensslErrorCheck(SSL *ssl, int retCode, bool &isError) {
    // 处理ssl的错误码
    int sslErr = SSL_get_error(ssl, retCode);
    isError = true;

    switch (sslErr) {
        case SSL_ERROR_WANT_READ:
            std::cout << "SSL_ERROR_WANT_READ" << std::endl;
            isError = false;
            break;
        case SSL_ERROR_WANT_WRITE:
            std::cout << "SSL_ERROR_WANT_WRITE" << std::endl;
            isError = false;
            break;
        case SSL_ERROR_NONE: // 没有错误发生，这种情况好像没怎么遇到过
            std::cout << "SSL_ERROR_WANT_WRITE" << std::endl;
            break;
        case SSL_ERROR_ZERO_RETURN:// == 0 ,代表对端关闭了连接
            std::cout << "SSL remote close the connection" << std::endl;
            break;
        case SSL_ERROR_SSL:
            std::cout << "SSL error:" << sslErr << std::endl;
            break;
        default:
            std::cout << "SSL unknown error:" << sslErr << std::endl;
            break;
    }
}

/** @fn
  * @brief
  * @param [in]socketFd: 客户端的socket文件句柄
  * @param [in]ctx：全局的上下文，保存有证书信息等
  * @return
  */
void onHandleClient(int socketFd, SSL_CTX *ctx) {
    std::cout << "new connection coming" << std::endl;

    SSL *ssl;
    const char reply[] = "test\n";

    // 基于ctx 产生一个新的SSL
    ssl = SSL_new(ctx);
    // 将连接用户的socket 加入到SSL
    SSL_set_fd(ssl, socketFd);

    auto t1 = std::chrono::steady_clock::now();
    // 建立SSL 连接
    int ret = SSL_accept(ssl);
    if (ret > 0) {
        std::cout << "ssl handshake success" << std::endl;
        // 发消息给客户端
        //SSL_write(ssl, reply, strlen(reply));

        auto t2 = std::chrono::steady_clock::now();
        auto timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        std::cout << "SSL_accept cost " << timeSpan.count() * 1000 << " ms." << std::endl;

        while (true) {
            char tempBuf[512] = {};
            int recvLen = SSL_read(ssl, tempBuf, sizeof(tempBuf));
            if (recvLen > 0) {
                std::cout << "客户端发来数据,len=" << recvLen << ",content=" << tempBuf << std::endl;

                // echo
                std::cout << "SSL_write " << std::string(tempBuf, recvLen) << std::endl;
                ret = SSL_write(ssl, tempBuf, recvLen);
                if (ret <= 0) {
                    std::cout << "SSL_write return <=0,ret=" << recvLen << std::endl;

                    bool isError = false;
                    opensslErrorCheck(ssl, recvLen, isError);

                    if (isError) {
                        std::cout << "SSL_write error,close" << std::endl;
                        break;
                    }
                }
            } else { // SSL_read <= 0 ，进一步检查openssl 的错误码，判断具体原因
                std::cout << "SSL_read return <=0,ret=" << recvLen << std::endl;

                bool isError = true;
                opensslErrorCheck(ssl, recvLen, isError);

                if (isError) {
                    std::cout << "SSL_read error,close" << std::endl;
                    break;
                }
            }

            /* 这里是TCP处理的流程，针对openssl，还需进一步针对<=0进行判断
             * else if (recvLen == 0) {
                std::cout << "客户端主动断开连接,退出接收流程" << std::endl;
                break;
            } else {
                std::cout << "发生其他错误,no=" << errno << ",desc=" << strerror(errno) << std::endl;
            }*/
        }
    } else {
        int code = SSL_get_error(ssl, ret);
        auto reason = ERR_reason_error_string(code);

        if (code == SSL_ERROR_SYSCALL) {
            std::cout << "ssl handshake error:errno=" << errno << ",reason:" << strerror(errno) << std::endl;
        } else {
            std::cout << "ssl handshake error:code=" << code << ",reason:" << reason << std::endl;
        }

        ERR_print_errors_fp(stderr);
    }

    std::cout << "cleanup ssl connection" << std::endl;
    // 关闭SSL 连接
    SSL_shutdown(ssl);
    // 释放SSL
    SSL_free(ssl);
    // 关闭socket
    close(socketFd);
}

int main() {
    int sockFd;
    SSL_CTX *ctx;

    // 捕获SIG_IGN信号，解决Broken pipe导致进程崩溃问题
    signal(SIGPIPE, SIG_IGN);

    init_openssl();
    ctx = create_context();

    configure_context(ctx, "../ssl/google.com.pem", "../ssl/google.com.key");
    //configure_context(ctx, "../ssl/zhaogang.com.pem", "../ssl/zhaogang.com.key");

    std::cout << "listen at :8433" << std::endl;
    sockFd = create_socket(8433);

    /* Handle connections */
    while (true) {
        struct sockaddr_in addr{};
        socklen_t len = sizeof(addr);

        // 阻塞，直到有新的连接到来
        int clientFd = accept(sockFd, (struct sockaddr *) &addr, &len);
        if (clientFd < 0) {
            perror("Unable to accept\n");
            break;
        }

        // 单独起1个线程处理客户端逻辑（错误的用法，这里只是为了演示，实战中需要使用epoll多路复用技术）
        std::thread task(onHandleClient, clientFd, ctx);
        task.detach();
    }

    // 关闭监听socket文件句柄
    close(sockFd);

    // 退出前释放全局的上下文
    SSL_CTX_free(ctx);

    // 清理openssl
    cleanup_openssl();

    return 0;
}