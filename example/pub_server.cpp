#include <pan/net/tcp.hpp>
#include <pan/net/pub_sub.hpp>

int main(int argc, char** argv)
{
    pan::net::pubsub::server<pan::net::pubsub::storage_base> server(8890);
    server.run();
    return 0;
}