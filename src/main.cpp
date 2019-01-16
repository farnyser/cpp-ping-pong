#include <iostream>
#include <chrono>
#include <boost/asio.hpp>

#include "latency.hpp"

using boost::asio::ip::udp;

int help()
{
    std::cout << "Usage:" << std::endl;
    std::cout << "\t" << "ping-pong [ping/pong] [udp/tcp] [host=127.0.0.1] [message=10000]" << std::endl;
    
    return -1;
}

int ping_udp(const std::string& host, size_t count);
int pong_udp(const std::string& host);
int ping_tcp(const std::string& host, size_t count);
int pong_tcp(const std::string& host);

int main(int argc, char **argv)
{
    if(argc <= 1)
        return help();

    auto mode = argc < 3 ? std::string("udp") : std::string(argv[2]);
    auto host = argc < 4 ? std::string("127.0.0.1") : std::string(argv[3]);
    auto count = argc < 5 ? 10000 : atol(argv[4]);

    if(mode == "udp")
    {
        if (argv[1] == std::string("ping"))
            return ping_udp(host, count);

        return pong_udp(host);
    }
    else if (mode == "tcp")
    {
        if (argv[1] == std::string("ping"))
            return ping_tcp(host, count);

        return pong_tcp(host);

    }

    return help();
}