#ifndef NET_HPP
#define NET_HPP

// TCP最大已建立连接队列大小
#define MAX_ESTABLISHED_NUM 10000
// TCP最大接收数据量大小（单位：Byte）
#define MAX_RECV_SIZE 1024
// 简单对称加密（固长密钥）
#define SECRET_KEY 0xAB
// 创建从Reactor的数量
#define SUB_REACTOR_NUM 2

#endif