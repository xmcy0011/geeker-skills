# SSL学习笔记

## OpenSSL库基础

根据 [官方的例子](https://wiki.openssl.org/index.php/Simple_TLS_Server)，OpenSSL常用的结构体和函数如下：

- 初始化OpenSSL库
  - SSL_library_init()：初始化SSL算法库函数
  - SSL_load_error_strings()：载入所有SSL 错误消息
  - OpenSSL_add_all_algorithms()： 载入所有SSL 算法
- 加载和验证证书
  - 通过SSL_CTX_new(SSL_METHOD *method)创建一个SSL_CTX *实例，用来保存证书的私钥，其中method通过SSLv23_server_method()指定，当然还有SSLv3_server_method()、

## 问题记录

### 获取错误描述

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



# 参考

- [OpenSSL官网：Simple TLS Server](https://wiki.openssl.org/index.php/Simple_TLS_Server)
- [IBM社区：Secure programming with the OpenSSL API（客户端）](https://developer.ibm.com/technologies/security/tutorials/l-openssl/)
- [SSL协议原理详解](http://cshihong.blog.csdn.net/article/details/90112705)
- [SSL原来介绍+OpenSSL API使用教程](http://blog.csdn.net/u013673367/article/details/23701279)
- [OpenSSL Libraries](https://www.openssl.org/docs/manmaster/man3/)
- [websocket协议实现及基于muduo库的功能扩展 https/ws/wss](https://blog.csdn.net/weixin_46572141/article/details/104894760)
- [websocket-muduo](https://github.com/chengwuloo/websocket)

