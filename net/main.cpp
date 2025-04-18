#include "reactor.hpp"
#include "Logger.hpp"

int main()
{
    LOG_INFO << "net test!";

    Reactor rctr("0.0.0.0", 4444);
}