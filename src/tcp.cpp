#include <iostream>
#include <chrono>
#include <boost/asio.hpp>

#include "latency.hpp"

using boost::asio::ip::tcp;

int ping_tcp(const std::string& host, size_t count)
{
    std::cout << "Ping " << host << " * " << count << " ..." << std::endl;

    auto l = latency<(std::uint64_t)1 * 1000 * 1000 * 1000>();

    boost::asio::io_service io_service;
    tcp::socket socket{io_service, tcp::endpoint(tcp::v4(), 20123)};

    char data[512];
    tcp::endpoint sender_endpoint = *(tcp::resolver{io_service}.resolve(tcp::resolver::query(host, "20124")));

    socket.connect(sender_endpoint);

    auto clock = std::chrono::high_resolution_clock{};

    for(size_t i = 0; i < count; i++)
    {
        auto now = clock.now();
        bool warm = i >= count/2;

        socket.send(boost::asio::buffer(data, 512));
        socket.receive(boost::asio::buffer(data, 512));

        if(warm)
            l.add(clock.now() - now);

        io_service.run();
        io_service.reset();
    }

    l.generate(std::cout);

    socket.close();
    io_service.run();
    io_service.reset();

    return 0;
}

int pong_tcp(const std::string& host)
{
    std::cout << "Pong " << host << " ..." << std::endl;

    boost::asio::io_service io_service;
    tcp::endpoint endpoint{tcp::v4(), 20124};

    tcp::acceptor acceptor{io_service, endpoint};

    char data[512];
    int flags = 0;
    boost::system::error_code error;

    while(true)
    {
        tcp::socket client_socket{io_service};
        acceptor.accept(client_socket);

        while(true)
        {
            client_socket.receive(boost::asio::buffer(data, 512), flags, error);
            if(error)
                break;

            client_socket.send(boost::asio::buffer(data, 512), flags, error);
            if(error)
                break;

            io_service.run();
            io_service.reset();
        }

        client_socket.close();
        io_service.run();
        io_service.reset();
    }

    return 0;
}