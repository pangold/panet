#include <pan/net/tcp.hpp>
#include <pan/net/pattern.hpp>

void on_topic(const std::string& topic, const std::string& content)
{
    LOG_INFO("Topic: %s", topic.c_str());
    LOG_INFO("Content: %s", content.c_str());
}

void on_subscribed(bool success, const std::string& topic)
{
    LOG_INFO("subscribe state: %s, topic: %s", success ? "success" : "failure", topic.c_str());
}

void on_unsubscribed(bool success, const std::string& topic)
{
    LOG_INFO("unsubscribe state: %s, topic: %s", success ? "success" : "failure", topic.c_str());
}

int main(int argc, char** argv)
{
    using namespace std::placeholders;
    pan::net::pattern::client client("localhost", 8890);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    client.register_topic_callback(std::bind(on_topic, _1, _2));
    client.subscribe("Test", std::bind(on_subscribed, _1, _2));
    client.unsubscribe("Test", std::bind(on_unsubscribed, _1, _2));
    client.subscribe("Test2", std::bind(on_subscribed, _1, _2));

    std::this_thread::sleep_for(std::chrono::seconds(1000));
    return 0;
}