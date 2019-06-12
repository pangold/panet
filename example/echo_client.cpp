#include <pan/net/tcp.hpp>
#include <pan/net/echo.hpp>

int main(int argc, char** argv)
{
    pan::net::tcp::client<pan::net::echo::client_handler> client("localhost", 8887);
    client.write("Hello");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}