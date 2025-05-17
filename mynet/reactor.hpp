#include "peer.hpp"
#include "syncQueue_nonblocking.hpp"
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <vector>
#include <thread>
#include <stop_token>
#include <atomic>
#include "handler.hpp"

#ifndef REACTOR_HPP
#define REACTOR_HPP

// SubReactor线程个数
#define SUB_REACTOR_NUM 2
// 单一SubReactor最大事件数
#define MAX_EVENTS_NUM 25

class Reactor : public Peer_ser // TCP Server
{
    int reactor_fd_;
    epoll_event acceptor_;
    std::atomic_flag is_running_;
    std::vector<std::jthread> subReactorVec_;
    SyncQueue_nonblocking<int> clientQue_;

public:
    Reactor(const std::string &my_ip,
            const int my_port)
        : Peer_ser(my_ip, my_port),
          reactor_fd_(-1)
    {
        if (-1 == fcntl(getFd(), F_SETFL, fcntl(getFd(), F_GETFL, 0) | O_NONBLOCK))
            throw std::runtime_error("fcntl failed");
        reactor_fd_ = epoll_create1(0);
        if (-1 == reactor_fd_)
            throw std::runtime_error("epoll_create1 failed");
        acceptor_.events = EPOLLIN | EPOLLET;
        acceptor_.data.fd = getFd();
        if (-1 == epoll_ctl(reactor_fd_, EPOLL_CTL_ADD, getFd(), &acceptor_))
            throw std::runtime_error("epoll_ctl failed");
        subReactorVec_.reserve(SUB_REACTOR_NUM);
        for (int i = 0; i < SUB_REACTOR_NUM; ++i)
            subReactorVec_.emplace_back(std::jthread([this](std::stop_token st)
                                                     {
                                                         int subReactor_fd = epoll_create1(0);
                                                         if (-1 == subReactor_fd)
                                                             throw std::runtime_error("epoll_create1 failed");
                                                         epoll_event events[MAX_EVENTS_NUM] = {};
                                                         epoll_event trigEvents[MAX_EVENTS_NUM] = {};
                                                         int curNum = 0;
                                                         auto data = new std::string;
                                                         Handler handler;
                                                         while (!st.stop_requested())
                                                         {
                                                             int cli_fd = -1;
                                                             if (0 == getClientQue().take_r(cli_fd))
                                                             {
                                                                 events[curNum].events = EPOLLIN | EPOLLET;
                                                                 events[curNum].data.fd = cli_fd;
                                                                 if (-1 == epoll_ctl(subReactor_fd, EPOLL_CTL_ADD, cli_fd, &events[curNum]))
                                                                 {
                                                                     perror("epoll_ctl");
                                                                     ::close(cli_fd);
                                                                 }
                                                             }
                                                             int n = epoll_wait(subReactor_fd, trigEvents, MAX_EVENTS_NUM, 10);// 0 millisecond timeout is high performance
                                                             if (-1 == n)
                                                             {
                                                                 perror("epoll_wait");
                                                                 break;
                                                             }
                                                             for (int i = 0; i < n; ++i)
                                                             {
                                                                 cli_fd = trigEvents[i].data.fd;
                                                                 std::string data(recv(cli_fd));
                                                                 if (data.empty())
                                                                 {
                                                                     epoll_ctl(subReactor_fd, EPOLL_CTL_DEL, cli_fd, nullptr);
                                                                     std::clog << "A client left, fd: " << cli_fd << std::endl;
                                                                     continue;
                                                                 }
                                                                 data = handler.process(data);
                                                                 if (!send(cli_fd, data))
                                                                 {
                                                                     epoll_ctl(subReactor_fd, EPOLL_CTL_DEL, cli_fd, nullptr);
                                                                     std::cerr << "failed to send to the client, fd: " << cli_fd << std::endl;
                                                                     continue;
                                                                 }
                                                             }
                                                         }
                                                         for (int i = 0; i < curNum; ++i)
                                                             epoll_ctl(subReactor_fd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
                                                         delete data;
                                                         ::close(subReactor_fd);
                                                         std::clog << "subReactor thread exit" << std::endl; //
                                                     }));
    }
    ~Reactor()
    {
        stop();
        ::close(reactor_fd_);
        for (auto &t : subReactorVec_)
            t.request_stop();
    }
    Reactor(const Reactor &) = delete;
    Reactor &operator=(const Reactor &) = delete;
    Reactor(Reactor &&) = delete;
    Reactor &operator=(Reactor &&) = delete;
    inline void run()
    {
        epoll_event ev;
        is_running_.test_and_set();
        while (is_running_.test())
        {
            if (1 == epoll_wait(reactor_fd_, &ev, 1, 10)) // 0 millisecond timeout is high performance
            {
                int cli_fd = accept();
                std::clog << "A new client was accepted, fd: " << cli_fd << std::endl;
                if (-1 == getClientQue().put_r(cli_fd))
                    send(cli_fd, "server is busy");
            }
        }
    }
    inline void stop() { is_running_.clear(); }
    inline SyncQueue_nonblocking<int> &getClientQue() { return clientQue_; }
};

#endif