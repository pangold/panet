#include <pan/net/echo.hpp>

int main(int argc, char** argv)
{
    pan::net::echo::server server(8887);
    server.run();
    return 0;
}