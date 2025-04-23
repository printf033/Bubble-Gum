#include "peer.hpp"
#include "reactor.hpp"
#include "Logger.hpp"
#include <glm/glm.hpp>
#include "converter.hpp"
Player me;
SyncQueue<NetPlayer> qu(100);
int main()
{
    LOG_INFO << "net test!";

    // Reactor rctr("0.0.0.0", 4444);
    // rctr.start();

    // Peer_udp udp("0.0.0.0", 4444);
    // while (1)
    // {
    //     LOG_DEBUG << "round";
    //     udp.send("255.255.255.255", 4444, "1111111111111111111111111111");
    //     std::this_thread::sleep_for(std::chrono::seconds(3));
    // }

    glm::mat4 m1(1.0f);
    LOG_INFO << '\n'
             << m1[0][0] << '#'
             << m1[1][0] << '#'
             << m1[2][0] << '#'
             << m1[3][0] << "#\n"
             << m1[0][1] << '#'
             << m1[1][1] << '#'
             << m1[2][1] << '#'
             << m1[3][1] << "#\n"
             << m1[0][2] << '#'
             << m1[1][2] << '#'
             << m1[2][2] << '#'
             << m1[3][2] << "#\n"
             << m1[0][3] << '#'
             << m1[1][3] << '#'
             << m1[2][3] << '#'
             << m1[3][3];
    LOG_INFO << Converter::convertMatrix2String(m1);
    glm::mat4 m2 = Converter::convertString2Matrix(Converter::convertMatrix2String(m1));
    LOG_INFO << Converter::convertMatrix2String(m2);
    LOG_INFO << (m1 == m2);
}