#include <pan/net/tcp.hpp>
#include <pan/net/proxy.hpp>

int main(int argc, char** argv)
{
    pan::net::tcp::server<pan::net::proxy::handler> proxy(8888);
    proxy.run();
    return 0;
}