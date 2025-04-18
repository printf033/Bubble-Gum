#include "Logger.hpp"
#include "peer.hpp"
#include "cachedThreadPool.hpp"
#include <event2/event.h>
#include <arpa/inet.h>

#ifndef REACTOR_HPP
#define REACTOR_HPP

class Reactor : public Peer_udp
{
    event_base *pReactor_;
    event *pAcceptor_;

public:
    Reactor(const std::string &my_ip,
            const int my_port)
        : Peer_udp(my_ip, my_port),
          pReactor_(nullptr),
          pAcceptor_(nullptr)
    {
        LOG_TRACE << "create";
        pReactor_ = event_base_new();
        if (nullptr == pReactor_)
            throw std::runtime_error("fail to create reactor");
        pAcceptor_ = event_new(
            pReactor_,
            getFd(),
            EV_READ | EV_PERSIST | EV_ET,
            [](int fd, short ev, void *arg)
            {
                Reactor *pReactor = static_cast<Reactor *>(arg);
                CachedThreadPool::_getInstance().excute([=]()
                                         {
                sockaddr_in ur_sockaddr_in;
                std::string data(pReactor->recv(ur_sockaddr_in));
                if (data.empty())
                {
                    LOG_ERROR << "recv failed from: "
                              << inet_ntoa(ur_sockaddr_in.sin_addr) << ":"
                              << ntohs(ur_sockaddr_in.sin_port);
                    return;
                }
                LOG_INFO << "recv: " << data << " from: "
                         << inet_ntoa(ur_sockaddr_in.sin_addr) << ":"
                         << ntohs(ur_sockaddr_in.sin_port);
                pReactor->send(inet_ntoa(ur_sockaddr_in.sin_addr), ntohs(ur_sockaddr_in.sin_port), data); });
            },
            this);
        if (nullptr == pAcceptor_)
            throw std::runtime_error("fail to new a Acceptor");
        if (-1 == event_add(pAcceptor_, nullptr))
            throw std::runtime_error("fail to add the Acceptor to the Reactor");
        event_base_dispatch(pReactor_);
    }
    ~Reactor()
    {
        event_del(pAcceptor_);
        event_free(pAcceptor_);
        event_base_free(pReactor_);
        LOG_TRACE << "destroy";
    }
};

#endif