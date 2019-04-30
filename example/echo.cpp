#include <cstdint>
#include <vector>
#include <iostream>
#include <pan/net.hpp>


using namespace pan::net;


void server(uint16_t port = 8888)
{
    try {
        echo::server server(port);
        server.run();
    }
    catch (std::exception& e) {
        std::cerr << "server: " << e.what() << std::endl;
    }
}

void client(const std::string host = "localhost", uint16_t port = 8888)
{
    try {
        echo::client client(host, port);
        int nnn = 10000;
        while (--nnn != 0) {
            client.handler().write(std::string("0123456789"));
        }
    }
    catch (std::exception& e) {
        std::cerr << "client: " << e.what() << std::endl;
    }
}

#if defined(EXAMPLE)

int main(int argc, char** argv)
{
#if defined(AS_SERVER)
    server();
#else
    client();
#endif // AS_SERVER
    return 0;
}

#endif // EXAMPLE
