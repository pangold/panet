#include <cstdint>
#include <vector>
#include <iostream>
//
#include <pan/net.hpp>

using namespace pan::net;


void server(uint16_t port = 8888)
{
    try {
        boost::asio::io_context io_context;
        echo::server_handler<tcp::session> handler;
        tcp::server server(io_context, port, handler);
        io_context.run();
    }
    catch (const std::exception& e) {
        std::cerr << "server: " << e.what() << std::endl;
    }
}

void client(const std::string host = "localhost", uint16_t port = 8888)
{
    try {
        boost::asio::io_context io_context;
        echo::client_handler<tcp::session> handler;
        tcp::client client(io_context, handler);

        if (!client.connect(host, port)) return;
        std::thread thread([&io_context]() { io_context.run(); });

        int nnn = 100000;
        while (--nnn != 0) {
            handler.write(std::string("0123456789"));
        }

        client.close();
        thread.join();
    }
    catch (const std::exception& e) {
        std::cerr << "client: " << e.what() << std::endl;
    }
}

template <typename T>
T add(T x, T y)
{
    return x + y;
}

template <typename T>
struct math {
    T add(T x, T y) { return x + y; }
    T minus(T x, T y) { return x - y; }
    T multiply(T x, T y) { return x * y; }
    T divide(T x, T y) { return x / y; }
};

void voidf(int x, int y)
{
    std::cout << "x: " << x << ", y: " << y << std::endl;
}

void rpc_server(uint16_t port = 8888)
{
    try {
        boost::asio::io_context io_context;
        rpc::server_handler<tcp::session> handler;
        tcp::server server(io_context, port, handler);
        math<int> m;
        handler.bind("add", add<int>);
        handler.bind("fadd", add<float>);
        handler.bind("dadd", add<double>);
        handler.bind("math.add", &math<int>::add, &m);
        handler.bind("math.minus", &math<int>::minus, &m);
        handler.bind("math.multiply", &math<int>::multiply, &m);
        handler.bind("math.divide", &math<int>::divide, &m);
        handler.bind("voidf", voidf);
        io_context.run();
    }
    catch (const std::exception& e) {
        std::cerr << "server: " << e.what() << std::endl;
    }
}

void rpc_client(const std::string host = "localhost", uint16_t port = 8888)
{
    try {
        boost::asio::io_context io_context;
        rpc::client_handler<tcp::session> handler;
        tcp::client client(io_context, handler);
        if (!client.connect(host, port)) return;

        std::thread thread([&io_context]() { io_context.run(); });

        float result2;
        double result3;
        int result1, result4, result5, result6, result7, result8;

        if (handler.call<int>(result1, "add", 200, 100)) {
            std::cout << "result1: " << result1 << std::endl;
        }

        if (handler.call<float>(result2, "fadd", 2.2f, 1.1f)) {
            std::cout << "result2: " << result2 << std::endl;
        }

        if (handler.call<double>(result3, "dadd", 2.6, 9.1)) {
            std::cout << "result3: " << result3 << std::endl;
        }

        if (handler.call<int>(result4, "math.add", 200, 100)) {
            std::cout << "result4: " << result4 << std::endl;
        }

        if (handler.call<int>(result5, "math.minus", 200, 100)) {
            std::cout << "result5: " << result5 << std::endl;
        }

        if (handler.call<int>(result6, "math.multiply", 200, 100)) {
            std::cout << "result6: " << result6 << std::endl;
        }

        if (handler.call<int>(result7, "math.divide", 200, 100)) {
            std::cout << "result7: " << result7 << std::endl;
        }

        if (handler.call<int>(result8, "voidf", 200, 100)) {
            std::cout << "result8: " << result8 << std::endl;
        }

        if (handler.call<int>(result8, "voiddf", 200, 100)) {
            std::cout << "result8: " << result8 << std::endl;
        }

        client.close();
        thread.join();
    }
    catch (const std::exception& e) {
        std::cerr << "client: " << e.what() << std::endl;
    }
}

int main(int argc, char** argv)
{
#if defined(AS_SERVER)
    rpc_server();
#else
    rpc_client();
#endif
    return 0;
}
