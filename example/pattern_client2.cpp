#include <pan/net/tcp.hpp>
#include <pan/net/pattern.hpp>

void on_published(bool success, const std::string& topic, const std::string& content)
{
    LOG_INFO("publish state: %s, topic: %s, content: %s", success ? "success" : "failure", topic.c_str(), content.c_str());
}

int main(int argc, char** argv)
{
    using namespace std::placeholders;
    pan::net::pattern::client client("localhost", 8890);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    client.publish("Test", "This is a test 1", std::bind(on_published, _1, _2, _3));
    client.publish("Test", "This is a test 2", std::bind(on_published, _1, _2, _3));
    client.publish("Test", "This is a test 3", std::bind(on_published, _1, _2, _3));
    client.publish("Test", "This is a test 4", std::bind(on_published, _1, _2, _3));
    client.publish("Test", "This is a test 5", std::bind(on_published, _1, _2, _3));
    client.publish("Test2", "This is a test 6", std::bind(on_published, _1, _2, _3));
    client.publish("Test2", "This is a test 7", std::bind(on_published, _1, _2, _3));
    client.publish("Test2", "This is a test 8", std::bind(on_published, _1, _2, _3));

    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}