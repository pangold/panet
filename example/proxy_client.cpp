#include <pan/net/tcp.hpp>
#include <pan/net/echo.hpp>
#include <pan/net/proxy.hpp>

int main(int argc, char** argv)
{
    pan::net::proxy::downstream client("localhost", 8888);

    client.write("echo", "Hello world.");
    client.write("echo", "Hey man, how is it going?");
    client.write("echo", "I'm great. How's yours?");
    client.write("echo", "Not bad.");
    client.write("echo", "Good.");

    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}