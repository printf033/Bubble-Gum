#include "peer.hpp"
#include <sys/socket.h>
#include <stdexcept>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <sstream>

namespace net
{
    Peer_cli::Peer_cli(const std::string &ur_ip, const int ur_port)
        : ur_fd_(-1), is_conn_(false) { conn(ur_ip, ur_port); }
    Peer_cli::Peer_cli()
        : ur_fd_(-1), is_conn_(false) {}
    Peer_cli::~Peer_cli() { disconn(); }
    void Peer_cli::setRecvTimer(const int sec)
    {
        struct timeval tv_struc;
        tv_struc.tv_sec = sec;
        tv_struc.tv_usec = 0;
        setsockopt(ur_fd_, SOL_SOCKET, SO_RCVTIMEO, &tv_struc, sizeof(tv_struc));
    }
    void Peer_cli::disconn()
    {
        if (-1 != ur_fd_)
            close(ur_fd_);
        ur_fd_ = -1;
        is_conn_ = false;
    }
    bool Peer_cli::conn(const std::string &ur_ip, const int ur_port)
    {
        disconn();
        ur_ip_ = ur_ip;
        ur_port_ = ur_port;
        ur_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == ur_fd_)
            return false;
        sockaddr_in ur_sockaddr_in = {};
        ur_sockaddr_in.sin_family = AF_INET;
        ur_sockaddr_in.sin_addr.s_addr = inet_addr(ur_ip_.c_str());
        ur_sockaddr_in.sin_port = htons(ur_port_);
        if (-1 == connect(ur_fd_, (const sockaddr *)&ur_sockaddr_in, sizeof(sockaddr_in)))
            return false;
        is_conn_ = true;
        setRecvTimer();
        return true;
    }
    bool Peer_cli::isConn() const { return is_conn_; }
    bool Peer_cli::send(const std::string &data)
    {
        if (!isConn() && !conn(ur_ip_, ur_port_))
            return false;
        const int total = data.size() + 1;
        int sum = 0;
        while (sum < total)
        {
            int n = ::send(ur_fd_, data.c_str() + sum, total - sum, 0);
            if (n <= 0)
            {
                if (!conn(ur_ip_, ur_port_))
                    return false;
                n = 0;
            }
            sum += n;
        }
        return true;
    }
    std::string Peer_cli::recv()
    {
        if (!isConn() && !conn(ur_ip_, ur_port_))
            return std::string();
        char buf[MAX_RECV_SIZE] = {};
        int n = ::recv(ur_fd_, buf, MAX_RECV_SIZE, 0);
        if (n > 0)
            return std::string(buf, n);
        disconn();
        return std::string();
    }
    std::string Peer_cli::interact(const std::string &data)
    {
        if (!isConn() && !conn(ur_ip_, ur_port_))
            return std::string();
        const int total = data.size() + 1;
        int sum = 0;
        while (sum < total)
        {
            int n = ::send(ur_fd_, data.c_str() + sum, total - sum, 0);
            if (n <= 0)
            {
                if (!conn(ur_ip_, ur_port_))
                    return std::string();
                n = 0;
            }
            sum += n;
        }
        char buf[MAX_RECV_SIZE] = {};
        int n = ::recv(ur_fd_, buf, MAX_RECV_SIZE, 0);
        if (n > 0)
            return std::string(buf, n);
        disconn();
        return std::string();
    }

    Peer_ser::Peer_ser(const std::string &my_ip, const int my_port)
        : my_fd_(socket(AF_INET, SOCK_STREAM, 0)),
          socklen_(sizeof(sockaddr_in))
    {
        if (-1 == my_fd_)
            throw std::runtime_error("fail to create a socket");
        sockaddr_in my_sockaddr_in = {};
        my_sockaddr_in.sin_family = AF_INET;
        my_sockaddr_in.sin_addr.s_addr = inet_addr(my_ip.c_str());
        my_sockaddr_in.sin_port = htons(my_port);
        if (-1 == bind(my_fd_, (const sockaddr *)&my_sockaddr_in, sizeof(sockaddr_in)))
            throw std::runtime_error("fail to bind the socket");
        if (-1 == listen(my_fd_, MAX_ESTABLISHED_NUM))
            throw std::runtime_error("fail to listen");
    }
    Peer_ser::~Peer_ser() { close(my_fd_); }
    int Peer_ser::getFd() const { return my_fd_; }
    int Peer_ser::accept()
    {
        sockaddr_in ur_sockaddr_in;
        int ur_fd = ::accept(my_fd_, (sockaddr *)&ur_sockaddr_in, &socklen_);
        if (ur_fd < 0)
            return -1;
        struct timeval tv_struc;
        tv_struc.tv_sec = 3; // 3s接收超时
        tv_struc.tv_usec = 0;
        setsockopt(ur_fd, SOL_SOCKET, SO_RCVTIMEO, &tv_struc, sizeof(tv_struc));
        return ur_fd;
    }
    bool Peer_ser::send(const int ur_fd, const std::string &data)
    {
        const int total = data.size() + 1;
        int sum = 0;
        while (sum < total)
        {
            int n = ::send(ur_fd, data.c_str() + sum, total - sum, 0);
            if (n <= 0)
            {
                close(ur_fd);
                return false;
            }
            sum += n;
        }
        return true;
    }
    std::string Peer_ser::recv(const int ur_fd)
    {
        char buf[MAX_RECV_SIZE] = {};
        if (::recv(ur_fd, buf, MAX_RECV_SIZE, 0) > 0)
            return std::string(buf);
        close(ur_fd);
        return std::string();
    }

    Peer_udp::Peer_udp(const std::string &my_ip, const int my_port)
        : my_fd_(socket(AF_INET, SOCK_DGRAM, 0)),
          socklen_(sizeof(sockaddr_in))
    {
        if (-1 == my_fd_)
            throw std::runtime_error("fail to create a socket");
        sockaddr_in my_sockaddr_in = {};
        my_sockaddr_in.sin_family = AF_INET;
        my_sockaddr_in.sin_addr.s_addr = inet_addr(my_ip.c_str());
        my_sockaddr_in.sin_port = htons(my_port);
        if (-1 == bind(my_fd_, (const sockaddr *)&my_sockaddr_in, sizeof(sockaddr_in)))
            throw std::runtime_error("fail to bind the socket");
    }
    Peer_udp::~Peer_udp() { close(my_fd_); }
    void Peer_udp::setRecvTimer(const int sec)
    {
        struct timeval tv_struc;
        tv_struc.tv_sec = sec;
        tv_struc.tv_usec = 0;
        setsockopt(my_fd_, SOL_SOCKET, SO_RCVTIMEO, &tv_struc, sizeof(tv_struc));
    }
    bool Peer_udp::send(const std::string &ur_ip, const int ur_port, const std::string &data)
    {
        sockaddr_in ur_sockaddr_in = {};
        ur_sockaddr_in.sin_family = AF_INET;
        ur_sockaddr_in.sin_addr.s_addr = inet_addr(ur_ip.c_str());
        ur_sockaddr_in.sin_port = htons(ur_port);
        const int total = data.size() + 1;
        int sum = 0;
        int times = 3;
        while (times-- > 0 && sum < total)
        {
            int n = ::sendto(my_fd_, data.c_str() + sum, total - sum, 0, (const sockaddr *)&ur_sockaddr_in, socklen_);
            if (n <= 0)
                return false;
            sum += n;
        }
        if (times < 0)
            return false;
        return true;
    }
    std::string Peer_udp::recv(std::string &ur_ip, int &ur_port)
    {
        sockaddr_in ur_sockaddr_in = {};
        ur_sockaddr_in.sin_family = AF_INET;
        ur_sockaddr_in.sin_addr.s_addr = inet_addr(ur_ip.c_str());
        ur_sockaddr_in.sin_port = htons(ur_port);
        char buf[MAX_RECV_SIZE] = {};
        if (::recvfrom(my_fd_, buf, MAX_RECV_SIZE, 0, (sockaddr *)&ur_sockaddr_in, &socklen_) > 0)
            return std::string(buf);
        return std::string();
    }
}