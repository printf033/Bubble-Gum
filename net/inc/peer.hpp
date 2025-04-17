#include "net.hpp"
#include <string>
#include <netinet/in.h>

#ifndef PEER_HPP
#define PEER_HPP

namespace net
{
    class Peer_cli
    {
        std::string ur_ip_;
        int ur_port_;
        int ur_fd_;
        bool is_conn_;

    public:
        Peer_cli(const std::string &ur_ip, const int ur_port);
        Peer_cli();
        ~Peer_cli();
        void setRecvTimer(const int sec = 30); // 30s接收超时
        void disconn();
        bool conn(const std::string &ur_ip, const int ur_port);
        bool isConn() const;

        bool send(const std::string &data);
        std::string recv();
        std::string interact(const std::string &data);
    };

    class Peer_ser
    {
        const int my_fd_;
        socklen_t socklen_;

    public:
        Peer_ser(const std::string &my_ip, const int my_port);
        ~Peer_ser();
        int getFd() const;

        int accept();
        bool send(const int ur_fd, const std::string &data);
        std::string recv(const int ur_fd); // 3s接收超时
    };

    class Peer_udp
    {
        const int my_fd_;
        socklen_t socklen_;

    public:
        Peer_udp(const std::string &my_ip, const int my_port);
        ~Peer_udp();
        void setRecvTimer(const int sec);

        bool send(const std::string &ur_ip, const int ur_port, const std::string &data);
        std::string recv(std::string &ur_ip, int &ur_port);
    };
}

#endif