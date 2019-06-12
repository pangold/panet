#include <pan/net/tcp.hpp>
#include <pan/net/pub_sub.hpp>

int main(int argc, char** argv)
{
    pan::net::pubsub::client<int> client("localhost", 8890);

    client.publish("Test", "This is a test 1");
    client.publish("Test", "This is a test 2");
    client.publish("Test", "This is a test 3");
    client.publish("Test", "This is a test 4");
    client.publish("Test", "This is a test 5");
    client.publish("Test", "This is a test 6");
    client.publish("Test", "This is a test 7");
    client.publish("Test", "This is a test 8");

    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}