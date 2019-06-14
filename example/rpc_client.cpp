#include <pan/base.hpp>
#include <pan/net/tcp.hpp>
#include <pan/net/rpc.hpp>

int main(int argc, char** argv)
{
    pan::net::rpc::client client("localhost", 8889);

    std::string error;
    int v1 = client.call2<int>(error, "add<int>", 200, 100);
    int v2 = client.call2<int>(error, "minus<int>", 200, 100);
    int v3 = client.call2<int>(error, "multiply<int>", 200, 100);
    int v4 = client.call2<int>(error, "divide<int>", 200, 100);
    std::cout << v1 << " " << v2 << " " << v3 << " " << v4 << std::endl;

    double vv1 = client.call<double>("add<double>", 220.1, 100.0);
    float vv2 = client.call<float>("add<float>", 210.99f, 100.f);
    int vv3 = client.call<int>("add<int>", 200, 100);
    double vv4 = client.call<double>("divide<double>", 200.0, 100.0);
    std::cout << vv1 << " " << vv2 << " " << vv3 << " " << vv4 << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    return 0;
}