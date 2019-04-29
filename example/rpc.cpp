#include <cstdint>
#include <vector>
#include <iostream>
#include <pan/net.hpp>
#include "math.hpp"

using namespace pan::net;

void bind(rpc::server& server, math<int>& m)
{
    server.handler().bind("add", add<int>);
    server.handler().bind("fadd", add<float>);
    server.handler().bind("dadd", add<double>);
    server.handler().bind("math.add", &math<int>::add, &m);
    server.handler().bind("math.minus", &math<int>::minus, &m);
    server.handler().bind("math.multiply", &math<int>::multiply, &m);
    server.handler().bind("math.divide", &math<int>::divide, &m);
    server.handler().bind("voidf", voidf);
}

// the way similate loacal function isn't very good. 
// think and how to make it.
void call(rpc::client& client)
{
    float result2;
    double result3;
    int result1, result4, result5, result6, result7, result8;

    if (client.handler().call<int>(result1, "add", 200, 100)) {
        std::cout << "result1: " << result1 << std::endl;
    }

    if (client.handler().call<float>(result2, "fadd", 2.2f, 1.1f)) {
        std::cout << "result2: " << result2 << std::endl;
    }

    if (client.handler().call<double>(result3, "dadd", 2.6, 9.1)) {
        std::cout << "result3: " << result3 << std::endl;
    }

    if (client.handler().call<int>(result4, "math.add", 200, 100)) {
        std::cout << "result4: " << result4 << std::endl;
    }

    if (client.handler().call<int>(result5, "math.minus", 200, 100)) {
        std::cout << "result5: " << result5 << std::endl;
    }

    if (client.handler().call<int>(result6, "math.multiply", 200, 100)) {
        std::cout << "result6: " << result6 << std::endl;
    }

    if (client.handler().call<int>(result7, "math.divide", 200, 100)) {
        std::cout << "result7: " << result7 << std::endl;
    }

    if (client.handler().call<int>(result8, "voidf", 200, 100)) {
        std::cout << "result8: " << result8 << std::endl;
    }

    if (client.handler().call<int>(result8, "voiddf", 200, 100)) {
        std::cout << "result8: " << result8 << std::endl;
    }
}

void rpc_server(uint16_t port = 8888)
{
    try {
        rpc::server server(port);
        math<int> m;
        bind(server, m);
        server.run();
    }
    catch (const std::exception& e) {
        std::cerr << "server: " << e.what() << std::endl;
    }
}

void rpc_client(const std::string host = "localhost", uint16_t port = 8888)
{
    try {
        rpc::client client(host, port);
        call(client);
    }
    catch (const std::exception& e) {
        std::cerr << "client: " << e.what() << std::endl;
    }
}

//int main(int argc, char** argv)
//{
//#if defined(AS_SERVER)
//    rpc_server();
//#else
//    rpc_client();
//#endif
//    return 0;
//}
