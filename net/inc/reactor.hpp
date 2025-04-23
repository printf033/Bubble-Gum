#include "Logger.hpp"
#include "peer.hpp"
#include "cachedThreadPool.hpp"
#include "syncQueue.hpp"
#include "player.hpp"
#include "converter.hpp"
#include <event2/event.h>
#include <arpa/inet.h>
#include <signal.h>
#include <jsoncpp/json/json.h>

#ifndef REACTOR_HPP
#define REACTOR_HPP

extern Player me;
extern SyncQueue<NetPlayer> qu;
void handler(int fd, short ev, void *arg);

class Reactor : public Peer_udp
{
    event_base *pReactor_;
    event *pSignal_; // close
    event *pAcceptor_;

public:
    Reactor(const std::string &my_ip,
            const int my_port)
        : Peer_udp(my_ip, my_port),
          pReactor_(nullptr),
          pSignal_(nullptr),
          pAcceptor_(nullptr)

    {
        LOG_TRACE << "create";
        pReactor_ = event_base_new();
        if (nullptr == pReactor_)
            throw std::runtime_error("fail to create reactor");
        pSignal_ = evsignal_new(
            pReactor_,
            SIGUSR1,
            [](int fd, short ev, void *arg)
            {
                Reactor *pReactor = static_cast<Reactor *>(arg);
                pReactor->stop();
            },
            this);
        if (nullptr == pSignal_)
            throw std::runtime_error("fail to new a Signal");
        if (-1 == event_add(pSignal_, nullptr))
            throw std::runtime_error("fail to add the Signal to the Reactor");
        pAcceptor_ = event_new(
            pReactor_,
            getFd(),
            EV_READ | EV_PERSIST | EV_ET,
            handler,
            this);
        if (nullptr == pAcceptor_)
            throw std::runtime_error("fail to new a Acceptor");
        if (-1 == event_add(pAcceptor_, nullptr))
            throw std::runtime_error("fail to add the Acceptor to the Reactor");
    }
    ~Reactor()
    {
        event_del(pAcceptor_);
        event_free(pAcceptor_);
        event_del(pSignal_);
        event_free(pSignal_);
        event_base_free(pReactor_);
        LOG_TRACE << "destroy";
    }
    void start() { event_base_dispatch(pReactor_); }
    void stop() { event_base_loopbreak(pReactor_); }
};

void handler(int fd, short ev, void *arg)
{
    assert(arg != nullptr);
    Reactor *pReactor = static_cast<Reactor *>(arg);
    std::string my_uuid(me.getUUIDv4());
    CachedThreadPool::_getInstance().excute([=]
                                            {
        sockaddr_in ur_sockaddr_in;
        std::string data(pReactor->recv(ur_sockaddr_in));
        if (data.empty())
            return;
        // //////////////////////////////////////////////
        // LOG_INFO << "recv: " << data << " from: "
        //          << inet_ntoa(ur_sockaddr_in.sin_addr) << ":"
        //          << ntohs(ur_sockaddr_in.sin_port);
        // //////////////////////////////////////////////
        Json::Reader jr;
        Json::Value jv;
        if (!jr.parse(data, jv))
            return;
        if (jv["uuid"].asString() != my_uuid)
        {
            qu.put_r(NetPlayer{
                .uuid = jv["uuid"].asString(),
                .globalTrans = Converter::convertString2Matrix(jv["globalTrans"].asString())
            });
        } });
}

#endif