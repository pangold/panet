#include <pan/net/tcp.hpp>
#include <pan/net/echo.hpp>

int main(int argc, char** argv)
{
    pan::net::tcp::server<pan::net::echo::server_handler> server(8887);
    server.run();
    return 0;
}