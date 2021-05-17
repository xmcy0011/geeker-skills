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

int main() {
    SSL_CTX *ctx = nullptr;

    // 初始化openssl
    init_openssl();
    std::cout << "init openssl success" << std::endl;

    // 初始化socket，同步连接远端服务器
    //int socketFd = create_socket("10.0.72.202", 8433);
    int socketFd = create_socket("10.0.72.202", 8000);
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

    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "exit ..." << std::endl;

    SSL_shutdown(ssl); // 关闭SSL连接
    SSL_free(ssl);     // 释放SSL资源
    close(socketFd);   // 关闭socket文件句柄
    SSL_CTX_free(ctx); // 释放SSL_CTX上下文资源

    return 0;
}