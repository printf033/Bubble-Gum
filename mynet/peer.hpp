#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdexcept>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <stdint.h>

// 前4字节（uint32_t）表示数据长度（Byte）
// 注意平台是否为大端序！且此文件中的类不可用于多线程！！！！！！！！！！！！

#ifndef PEER_HPP
#define PEER_HPP

// TCP最大已建立连接队列大小
#define MAX_ESTABLISHED_NUM 100
// TCP客户端接收等待超时时间（s）
#define CLINET_RECV_TIMEOUT 30
// TCP服务端接收等待超时时间（s）
#define SERVER_RECV_TIMEOUT 3
// UDP最大一次接收数据大小（Byte）
#define MAX_RECV_SIZE 1024

class Peer_cli
{
    std::string ur_ip_;
    int ur_port_;
    int ur_fd_;
    bool is_conn_;

public:
    Peer_cli(const std::string &ur_ip, const int ur_port)
        : ur_fd_(-1), is_conn_(false) { conn(ur_ip, ur_port); }
    Peer_cli()
        : ur_fd_(-1), is_conn_(false) {}
    ~Peer_cli() { disconn(); }
    Peer_cli(const Peer_cli &) = delete;
    Peer_cli &operator=(const Peer_cli &) = delete;
    Peer_cli(Peer_cli &&) = delete;
    Peer_cli &operator=(Peer_cli &&) = delete;
    void setRecvTimer(const int sec = CLINET_RECV_TIMEOUT)
    {
        struct timeval tv_struc;
        tv_struc.tv_sec = sec;
        tv_struc.tv_usec = 0;
        setsockopt(ur_fd_, SOL_SOCKET, SO_RCVTIMEO, &tv_struc, sizeof(tv_struc));
    }
    void disconn()
    {
        if (-1 != ur_fd_)
            close(ur_fd_);
        ur_fd_ = -1;
        is_conn_ = false;
    }
    bool conn(const std::string &ur_ip, const int ur_port)
    {
        disconn();
        ur_ip_ = ur_ip;
        ur_port_ = ur_port;
        ur_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == ur_fd_)
            return false;
        sockaddr_in ur_sockaddr_in{};
        ur_sockaddr_in.sin_family = AF_INET;
        ur_sockaddr_in.sin_addr.s_addr = inet_addr(ur_ip_.c_str());
        ur_sockaddr_in.sin_port = htons(ur_port_);
        if (-1 == connect(ur_fd_, (const sockaddr *)&ur_sockaddr_in, sizeof(sockaddr_in)))
            return false;
        is_conn_ = true;
        setRecvTimer();
        return true;
    }
    bool isConn() const { return is_conn_; }
    bool send(const std::string &data)
    {
        if (!isConn() && !conn(ur_ip_, ur_port_))
            return false;
        uint32_t len = data.size() + 1;
        ::send(ur_fd_, &len, 4, 0);
        size_t sum = 0;
        while (sum < len)
        {
            ssize_t n = ::send(ur_fd_, data.c_str() + sum, len - sum, 0);
            if (n < 0)
            {
                if (errno == EINTR)
                    continue;
                disconn();
                return false;
            }
            sum += static_cast<size_t>(n);
        }
        return true;
    }
    std::string recv()
    {
        if (!isConn() && !conn(ur_ip_, ur_port_))
            return {};
        uint32_t len = 0;
        size_t sum = 0;
        while (sum < 4)
        {
            ssize_t n = ::recv(ur_fd_, (char *)&len + sum, 4 - sum, 0);
            if (n <= 0)
            {
                if (n < 0 && errno == EINTR)
                    continue;
                disconn();
                return {};
            }
            sum += static_cast<size_t>(n);
        }
        std::string data;
        data.resize(len);
        sum = 0;
        while (sum < len)
        {
            ssize_t n = ::recv(ur_fd_, data.data() + sum, len - sum, 0);
            if (n <= 0)
            {
                if (n < 0 && errno == EINTR)
                    continue;
                disconn();
                return {};
            }
            sum += static_cast<size_t>(n);
        }
        return data;
    }
    std::string interact(const std::string &data)
    {
        if (!send(data))
            return {};
        return recv();
    }
};

class Peer_ser
{
    const int my_fd_;
    socklen_t socklen_;

public:
    Peer_ser(const std::string &my_ip, const int my_port)
        : my_fd_(socket(AF_INET, SOCK_STREAM, 0)),
          socklen_(sizeof(sockaddr_in))
    {
        if (-1 == my_fd_)
            throw std::runtime_error("fail to create a socket");
        sockaddr_in my_sockaddr_in{};
        my_sockaddr_in.sin_family = AF_INET;
        my_sockaddr_in.sin_addr.s_addr = inet_addr(my_ip.c_str());
        my_sockaddr_in.sin_port = htons(my_port);
        if (-1 == bind(my_fd_, (const sockaddr *)&my_sockaddr_in, sizeof(sockaddr_in)))
            throw std::runtime_error("fail to bind the socket");
        if (-1 == listen(my_fd_, MAX_ESTABLISHED_NUM))
            throw std::runtime_error("fail to listen");
    }
    ~Peer_ser() { close(my_fd_); }
    Peer_ser(const Peer_ser &) = delete;
    Peer_ser &operator=(const Peer_ser &) = delete;
    Peer_ser(Peer_ser &&) = delete;
    Peer_ser &operator=(Peer_ser &&) = delete;
    int getFd() const { return my_fd_; }
    int accept()
    {
        sockaddr_in ur_sockaddr_in;
        int ur_fd = ::accept(my_fd_, (sockaddr *)&ur_sockaddr_in, &socklen_);
        if (ur_fd < 0)
            return -1;
        struct timeval tv_struc;
        tv_struc.tv_sec = SERVER_RECV_TIMEOUT;
        tv_struc.tv_usec = 0;
        setsockopt(ur_fd, SOL_SOCKET, SO_RCVTIMEO, &tv_struc, sizeof(tv_struc));
        return ur_fd;
    }
    bool send(const int ur_fd, const std::string &data)
    {
        uint32_t len = data.size() + 1;
        ::send(ur_fd, &len, 4, 0);
        size_t sum = 0;
        while (sum < len)
        {
            ssize_t n = ::send(ur_fd, data.c_str() + sum, len - sum, 0);
            if (n < 0)
            {
                if (errno == EINTR)
                    continue;
                close(ur_fd);
                return false;
            }
            sum += static_cast<size_t>(n);
        }
        return true;
    }
    std::string recv(const int ur_fd)
    {
        uint32_t len = 0;
        size_t sum = 0;
        while (sum < 4)
        {
            ssize_t n = ::recv(ur_fd, (char *)&len + sum, 4 - sum, 0);
            if (n <= 0)
            {
                if (n < 0 && errno == EINTR)
                    continue;
                close(ur_fd);
                return {};
            }
            sum += static_cast<size_t>(n);
        }
        std::string data;
        data.resize(len);
        sum = 0;
        while (sum < len)
        {
            ssize_t n = ::recv(ur_fd, data.data() + sum, len - sum, 0);
            if (n <= 0)
            {
                if (n < 0 && errno == EINTR)
                    continue;
                close(ur_fd);
                return {};
            }
            sum += static_cast<size_t>(n);
        }
        return data;
    }
};

class Peer_udp
{
    const int my_fd_;
    socklen_t socklen_;

public:
    Peer_udp(const std::string &my_ip, const int my_port)
        : my_fd_(socket(AF_INET, SOCK_DGRAM, 0)),
          socklen_(sizeof(sockaddr_in))
    {
        if (-1 == my_fd_)
            throw std::runtime_error("fail to create a socket");
        sockaddr_in my_sockaddr_in{};
        my_sockaddr_in.sin_family = AF_INET;
        my_sockaddr_in.sin_addr.s_addr = inet_addr(my_ip.c_str());
        my_sockaddr_in.sin_port = htons(my_port);
        if (-1 == bind(my_fd_, (const sockaddr *)&my_sockaddr_in, sizeof(sockaddr_in)))
            throw std::runtime_error("fail to bind the socket");
        int broadcastEnable = 1;
        setsockopt(my_fd_, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    }
    ~Peer_udp() { close(my_fd_); }
    Peer_udp(const Peer_udp &) = delete;
    Peer_udp &operator=(const Peer_udp &) = delete;
    Peer_udp(Peer_udp &&) = delete;
    Peer_udp &operator=(Peer_udp &&) = delete;
    int getFd() const { return my_fd_; }
    void send(sockaddr_in &ur_sockaddr_in, const std::string &data)
    {
        ::sendto(my_fd_, data.c_str(), data.size() + 1, 0, (const sockaddr *)&ur_sockaddr_in, socklen_);
    }
    void send(const std::string &ur_ip, const int ur_port, const std::string &data)
    {
        sockaddr_in ur_sockaddr_in{};
        ur_sockaddr_in.sin_family = AF_INET;
        ur_sockaddr_in.sin_addr.s_addr = inet_addr(ur_ip.c_str());
        ur_sockaddr_in.sin_port = htons(ur_port);
        send(ur_sockaddr_in, data);
    }
    std::string recv(sockaddr_in &ur_sockaddr_in)
    {
        char buf[MAX_RECV_SIZE]{};
        if (::recvfrom(my_fd_, buf, MAX_RECV_SIZE, 0, (sockaddr *)&ur_sockaddr_in, &socklen_) <= 0)
            return std::string();
        return std::string(buf);
    }
};

#endif