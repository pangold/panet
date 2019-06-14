#include <pan/net/tcp.hpp>
#include <pan/net/echo.hpp>
#include <pan/net/proxy.hpp>

int main(int argc, char** argv)
{
    pan::net::echo::datagram_client client("localhost", 8888);
    client.write_datagram("echo", "Hello world.");
    client.write_datagram("echo", "Hey man, how is it going?");
    client.write_datagram("echo", "I'm great. How's yours?");
    client.write_datagram("echo", "Not bad.");
    client.write_datagram("echo", "Good.");

    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}