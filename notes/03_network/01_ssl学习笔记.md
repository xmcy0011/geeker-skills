# SSL学习笔记

## OpenSSL库基础

根据 [官方的例子](https://wiki.openssl.org/index.php/Simple_TLS_Server)，OpenSSL常用的结构体和函数如下：

- 初始化OpenSSL库
  - SSL_library_init()：初始化SSL算法库函数
  - SSL_load_error_strings()：载入所有SSL 错误消息
  - OpenSSL_add_all_algorithms()： 载入所有SSL 算法
- 加载和验证证书
  - 通过SSL_CTX_new(SSL_METHOD *method)创建一个SSL_CTX *实例，用来保存证书的私钥。其中method通过TLS_client_method()、TLS_server_method() 和TLS_method()创建（一些写法中会提到SSLv23_method(),SSLv23_server_method(), SSLv23_client_method()等API，[官方文档](https://www.openssl.org/docs/manmaster/man3/SSLv23_method.html) 明确提到已经过时了）。
  - 通过SSL_CTX_use_certificate_file()和SSL_CTX_use_PrivateKey_file()载入证书和私钥，SSL_CTX_check_private_key()来检查是否匹配。
- 创建SSL实例，绑定socket
  - SSL_new(ctx)来创建一个ssl的实例，这个实例后续握手、读写、关闭等需要
  - 通过SSL_set_fd()来把SSL实力和socket句柄关联起来
- SSL握手，有2种方式
  - SSL_accept()，被动的等待客户端，注意这里会阻塞
  - SSL_do_handshake
- 收发
  - SSL_read()/SSL_write()来进行数据加解密和读写
  - 注意需要对返回值调用SSL_get_error()进一步判断，如果返回的是SSL_ERROR_WANT_READ,SSL_ERROR_WANT_WRITE代表需要再次调用
- 释放
  - 释放ssl实例，SSL_shutdown()来关闭ssl连接，SSL_free()释放内存，别忘记还要调用close()关闭socket句柄
  - 释放ctx上下文实例，因为ctx是全局的，应该在程序退出前调用SSL_CTX_free()进行释放

## Demo

官方的例子可以参考：[OpenSSL官网：Simple TLS Server](https://wiki.openssl.org/index.php/Simple_TLS_Server)

我的代码可以参见 code/tcp/05-simple-multi-thread-in-ssl/server/ssl_server.cpp



### Server

```cpp
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
```



### Client

```c++
/** @file main.h
  * @brief 
  * @author teng.qing
  * @date 2021/5/13
  */

// openssl
#include <openssl/ssl.h>
#include <openssl/err.h>

// socket
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <resolv.h>

#include <iostream>
#include <thread>

void showCerts(SSL *ssl) {
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl);
    if (cert != nullptr) {
        std::cout << "数字证书信息:" << std::endl;
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        std::cout << "证书: " << line << std::endl;
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        std::cout << "颁发者: " << line << std::endl;
        free(line);
        X509_free(cert);
    } else {
        std::cout << "无证书信息！" << std::endl;
    }
}

/** @fn init_openssl
  * @brief 全局初始化openssl库，只需要调用一次
  * @return
  */
void init_openssl() {
    //SSL_library_init();
    SSL_load_error_strings();       // 载入所有SSL 错误消息
    OpenSSL_add_all_algorithms();   // 加载所有支持的算法
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
    method = SSLv23_client_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

/** @fn create_socket
  * @brief 创建一个监听的socket
  * @param [in]listenPort:监听端口
  * @return
  */
int create_socket(std::string serverIp, uint16_t serverPort) {
    int sockFd = 0;
    struct sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(serverPort);
    addr.sin_addr.s_addr = inet_addr(serverIp.c_str());

    sockFd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        printf("create socket error:%d", errno);
        exit(EXIT_FAILURE);
    }

    int ret = connect(sockFd, (struct sockaddr *) &addr, sizeof(sockaddr_in));
    if (ret != 0) {
        std::cout << "Connect err:" << errno << std::endl;
        exit(errno);
    }

    return sockFd;
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

int main() {
    SSL_CTX *ctx = nullptr;

    // 初始化openssl
    init_openssl();
    std::cout << "init openssl success" << std::endl;

    // 初始化socket，同步连接远端服务器
    //int socketFd = create_socket("10.0.72.202", 8433);
    int socketFd = create_socket("10.80.0.17", 8000);
    std::cout << "tcp connect remote success" << std::endl;

    // 创建SSL_CTX上下文
    ctx = create_context();

    // 绑定socket句柄到SSL实例上
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, socketFd);

    // 建立SSL链接，握手
    std::cout << "SSL_connect 2s later will connect and do hand shake..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "SSL_connect " << std::endl;
    int ret = SSL_connect(ssl);
    if (ret <= 0) {
        ERR_print_errors_fp(stderr);
        return 0;
    }
    std::cout << "handshake success" << std::endl;

    // 显示对方证书信息
    std::cout << "Connected with " << SSL_get_cipher(ssl) << " encryption" << std::endl;
    showCerts(ssl);

    std::cout << "send hello server" << std::endl;

    std::string msg = "hello serve";
    SSL_write(ssl, msg.c_str(), msg.length());

    // wait server response
    char tempBuf[256] = {};
    ret = SSL_read(ssl, tempBuf, sizeof(tempBuf));
    if (ret <= 0) {
        std::cout << "SSL_read return <=0,ret=" << ret << std::endl;

        bool isError = false;
        opensslErrorCheck(ssl, ret, isError);

        if (isError) {
            std::cout << "SSL_read error,close" << std::endl;
        }
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "exit ..." << std::endl;

    SSL_shutdown(ssl); // 关闭SSL连接
    SSL_free(ssl);     // 释放SSL资源
    close(socketFd);   // 关闭socket文件句柄
    SSL_CTX_free(ctx); // 释放SSL_CTX上下文资源

    return 0;
}
```



### 证书生成

生成自签名证书命令如下：
1. 生成私钥
```bash
$ openssl genrsa -out google.com.key 2048
```

2. 生成CSR（证书签名请求）
```bash
$ openssl req -new -out google.com.csr -key google.com.key

Country Name (2 letter code) [AU]:CN
State or Province Name (full name) [Some-State]:Shanghai
Locality Name (eg, city) []:Shanghai
Organization Name (eg, company) [Internet Widgits Pty Ltd]:Google Ltd
Organizational Unit Name (eg, section) []:google.com
Common Name (e.g. server FQDN or YOUR name) []:*.google.com #这一项必须和你的域名一致
Email Address []:kefu@google.com
A challenge password []:fG!#tRru
An optional company name []:Google.com
```

3. 生成自签名证书（100年过期）
```bash
$ openssl x509 -req -in google.com.csr -out google.com.cer -signkey google.com.key -CAcreateserial -days 36500
```

4. 生成服务器crt格式证书
```bash
$ openssl x509 -inform PEM -in google.com.cer -out google.com.crt
```
5. 生成PEM公钥
```bash
$ openssl x509 -in google.com.crt -outform PEM -out google.com.pem
```

最后，google.com.pem 和 google.com.key 是本程序需要的 公钥和私钥

附录：
- 生成IOS客户端p12格式根证书（输入密码fG!#tRru）
```bash
$ openssl pkcs12 -export -clcerts -in google.com.cer -inkey google.com.key -out google.com.p12
```
- 生成Android客户端bks格式证书
```bash
# 略
```



## 问题记录

### 获取错误原因

SSL_get_error(),ERR_reason_error_string()

```c++
int ret = SSL_accept(ssl);
if (ret <= 0) {
  int code = SSL_get_error(ssl, ret);
  auto reason = ERR_reason_error_string(code);
  std::cout << "ssl handshake error:code=" << code << ",reason:" << reason << std::endl;
  ERR_print_errors_fp(stderr);
}
```

### 数据发送对方接受不到

当 SSL_read 和 SSL_write 返回值<=0时，需要调用SSL_get_error(ssl, ret)再次判断，如果是SSL_ERROR_WANT_READ 或者 SSL_ERROR_WANT_WRITE，则需要再次调用SSL_read或者SSL_write。

拿我修改的evpp增加openssl代码举例：

```c++
// add openssl support
int serrno = 0;
ssize_t n = !ssl_ ?
  input_buffer_.ReadFromFD(chan_->fd(), &serrno) :
evpp::ssl::SSL_read(ssl_, &input_buffer_, &serrno);
if (n > 0) {
  msg_fn_(shared_from_this(), &input_buffer_);
} else if (ssl_) { // <= 0 , 需要进一步处理SSL错误码
  // deal SSL_ERROR_WANT_READ/SSL_ERROR_WANT_WRITE
  // 这里如果不这么处理，则导致接收数据不完整，从而出错
  switch (serrno) {
    case SSL_ERROR_WANT_READ:
      chan_->EnableReadEvent();
      break;
    case SSL_ERROR_WANT_WRITE:
      chan_->EnableWriteEvent();
      break;
    case SSL_ERROR_ZERO_RETURN://SSL has been shutdown，相当于原生write()返回0，代表对端关闭连接（正常情况）
      DLOG_TRACE << "SSL has been shutdown(" << serrno << ").";
      HandleError();
      break;
    case SSL_ERROR_SSL:
      DLOG_TRACE << "SSL has error(" << serrno << ").";
      HandleError();
      break;
    case SSL_ERROR_NONE: // 0，have none error
      DLOG_TRACE << "SSL has error none";
      break;
    default:
      DLOG_TRACE << "SSL Connection has been aborted(" << serrno << ").";
      HandleError();
      break;
  }
} else if (n == 0) { // remote close the connection
  if (type() == kOutgoing) {
    // This is an outgoisslsssng connection, we own it and it's done. so close it
    DLOG_TRACE << "fd=" << fd_ << ". We read 0 bytes and close the socket.";
    status_ = kDisconnecting;
    HandleClose();
  } else {
    // Fix the half-closing problem : https://github.com/chenshuo/muduo/pull/117

    chan_->DisableReadEvent();
    if (close_delay_.IsZero()) {
      DLOG_TRACE << "channel (fd=" << chan_->fd() << ") DisableReadEvent. delay time "
        << close_delay_.Seconds() << "s. We close this connection immediately";
      DelayClose();
    } else {
      // This is an incoming connection, we need to preserve the
      // connection for a while so that we can reply to it.
      // And we set a timer to close the connection eventually.
      DLOG_TRACE << "channel (fd=" << chan_->fd()
        << ") DisableReadEvent. And set a timer to delay close this TCPConn, delay time "
        << close_delay_.Seconds() << "s";
      delay_close_timer_ = loop_->RunAfter(close_delay_, std::bind(&TCPConn::DelayClose,
                                                                   shared_from_this())); // TODO leave it to user layer close.
    }
  }
} else { // n < 0
  if (EVUTIL_ERR_RW_RETRIABLE(serrno)) {
    DLOG_TRACE << "errno=" << serrno << " " << strerror(serrno);
  } else {
    DLOG_TRACE << "errno=" << serrno << " " << strerror(serrno)
      << " We are closing this connection now.";
    HandleError();
  }
}
```

### SSL_connect在服务端异常时阻塞卡住

如果connect()系统调用成功，但是调用SSL_connect的时候卡住，则代表对方没有响应我们的握手请求，可能是出BUG了或者其他原因，此时我们需要一种机制能监测出来。通常有2种思路：

- 通过定时器，超过一定时间调用SSL_shutdown(ssl)，SSL_connect()会立即返回。
- 通过非阻塞I/O+select I/O复用增加重试机制达到超时监测的目标。

2种方案各有优劣，在evpp增加openssl的支持中，我使用了第二种方式（参考[Stackoverflow](https://stackoverflow.com/questions/28508374/ssl-connect-for-non-blocking-socket)）：

```c++
// no-blocking io
std::random_device rd;
std::default_random_engine gen = std::default_random_engine(rd());
std::uniform_int_distribution<int> dis(5, 10); // c++11 的随机数
int maxTimes = dis(gen); // 最大次数,5 - 10 次
int curTimes = 0;
int ret = -1;

// 这里限制超时时间
while (curTimes < maxTimes) { // 100 - 200 ms超时
  ret = SSL_connect(ssl);
  if (ret == -1) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sockfd, &fds);
    int ret = SSL_get_error(ssl, -1);
    switch (ret) {
      case SSL_ERROR_WANT_READ:
      case SSL_ERROR_WANT_WRITE: {
        struct timeval t{};
        t.tv_sec = 0;
        t.tv_usec = 100 * 1000; // 100 ms
        select(sockfd + 1, &fds, nullptr, nullptr, &t);
        curTimes++;
        break;
      }
      default:
        break;
    }
  } else {
    break;
  }
}

if (ret <= 0) {
  LOG_WARN << "SSL_connect error:" << ret;
  conn_fn_(TCPConnPtr(new TCPConn(loop_, "", sockfd, laddr, remote_addr_, 0)));
  return;
}
```



# 参考

- [OpenSSL官网：Simple TLS Server](https://wiki.openssl.org/index.php/Simple_TLS_Server)
- [IBM社区：Secure programming with the OpenSSL API（客户端）](https://developer.ibm.com/technologies/security/tutorials/l-openssl/)
- [SSL协议原理详解](http://cshihong.blog.csdn.net/article/details/90112705)
- [SSL原来介绍+OpenSSL API使用教程](http://blog.csdn.net/u013673367/article/details/23701279)
- [OpenSSL Libraries](https://www.openssl.org/docs/manmaster/man3/)
- [websocket协议实现及基于muduo库的功能扩展 https/ws/wss](https://blog.csdn.net/weixin_46572141/article/details/104894760)
- [websocket-muduo](https://github.com/chengwuloo/websocket)

