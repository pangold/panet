#include <pan/net/tcp.hpp>
#include <pan/net/pub_sub.hpp>

void on_topic(const std::shared_ptr<Pango::PubSub::Topic> topic)
{
    LOG_INFO("Topic: %s", topic->name().c_str());
    LOG_INFO("Received at: %lld", topic->datetime());
    LOG_INFO("Content: %s", topic->content().c_str());
    LOG_INFO("Who: %s", topic->who().c_str());
    LOG_INFO("Number: %d", topic->number());
}

void on_subscribed(const std::string& topic_name)
{
    LOG_INFO("Subscribe: %s successfully", topic_name.c_str());
}

int main(int argc, char** argv)
{
    pan::net::pubsub::client client("localhost", 8890);

    client.register_topic_callback(std::bind(on_topic, std::placeholders::_1));
    client.subscribe("Test", std::bind(on_subscribed, std::placeholders::_1));

    std::this_thread::sleep_for(std::chrono::seconds(1000));
    return 0;
}