#include <pan/base.hpp>
#include <pan/net/tcp.hpp>
#include <pan/net/rpc.hpp>
#include "math.hpp"

int main(int argc, char* argv)
{
    math<int> imath;
    pan::net::rpc::server server(8889);
    server.bind("math.add<int>", &math<int>::add, &imath);
    server.bind("math.minus<int>", &math<int>::minus, &imath);
    server.bind("math.multiply<int>", &math<int>::multiply, &imath);
    server.bind("math.divide<int>", &math<int>::divide, &imath);
    server.bind("add<int>", add<int>);
    server.bind("minus<int>", minus<int>);
    server.bind("multiply<int>", multiply<int>);
    server.bind("divide<int>", divide<int>);
    server.bind("add<float>", add<float>);
    server.bind("minus<float>", minus<float>);
    server.bind("multiply<float>", multiply<float>);
    server.bind("divide<float>", divide<float>);
    server.bind("add<double>", add<double>);
    server.bind("minus<double>", minus<double>);
    server.bind("multiply<double>", multiply<double>);
    server.bind("divide<double>", divide<double>);
    server.run();
    
    return 0;
}