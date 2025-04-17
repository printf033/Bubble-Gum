#include "peer.hpp"
#include "Logger.hpp"

int main()
{
    LOG_INFO << "udp test!";

    net::Peer_udp udp("0.0.0.0", 5555);

    while (1)
    {
        std::string input;
        std::cout << "对方端口：\n";
        std::cin >> input;
        int port = std::stoi(input);
        std::cout << "内容：\n";
        std::cin >> input;
        std::string ip("127.0.0.1");
        udp.send(ip, port, input);
        std::cout << "对方返回：\n"
                  << udp.recv(ip, port) << std::endl;
    }
}