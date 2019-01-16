#include <iostream>
#include <chrono>
#include <boost/asio.hpp>

#include "latency.hpp"

using boost::asio::ip::udp;

int ping_udp(const std::string& host, size_t count)
{
    std::cout << "Ping " << host << " * " << count << " ..." << std::endl;

    auto l = latency<(std::uint64_t)1 * 1000 * 1000 * 1000>();

    boost::asio::io_service io_service;
    udp::socket socket{io_service, udp::endpoint(udp::v4(), 20123)};

    char data[512];
    udp::endpoint sender_endpoint = *(udp::resolver{io_service}.resolve(udp::resolver::query(host, "20124")));
    udp::endpoint dummy;

    auto clock = std::chrono::high_resolution_clock{};

    for(size_t i = 0; i < count; i++)
    {
        auto now = clock.now();
        bool warm = i >= count/2;

        socket.async_send_to(boost::asio::buffer(data, 512), sender_endpoint,
                             [&](const boost::system::error_code &error, size_t bytes_recvd)
                             {
                                 socket.async_receive_from(boost::asio::buffer(data, 512), dummy, [&](const boost::system::error_code &error, size_t bytes_recvd) {
                                     if(warm)
                                        l.add(clock.now() - now);
                                 });
                             });

        io_service.run();
        io_service.reset();
    }

    l.generate(std::cout);

    socket.close();
    return 0;
}

int pong_udp(const std::string& host)
{
    std::cout << "Pong " << host << " ..." << std::endl;

    boost::asio::io_service io_service;
    udp::socket socket{io_service, udp::endpoint(udp::v4(), 20124)};

    char data[512];
    udp::endpoint sender_endpoint;


    while(true)
    {
        socket.async_receive_from(boost::asio::buffer(data, 512), sender_endpoint, [&](const boost::system::error_code& error, size_t bytes_recvd){
            socket.async_send_to(boost::asio::buffer(data, 512), sender_endpoint, [](const boost::system::error_code& error, size_t bytes_recvd){});
        });

        io_service.run();
        io_service.reset();
    }

    socket.close();
    return 0;
}