#include <pan/net/tcp.hpp>
#include <pan/net/pattern.hpp>

int main(int argc, char** argv)
{
    pan::net::pattern::server server(8890);
    server.run();
    return 0;
}