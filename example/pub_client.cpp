#include <pan/net/tcp.hpp>
#include <pan/net/pub_sub.hpp>

void on_published(const std::string& topic_name, const std::string& content)
{
    LOG_INFO("Published [topic: %s, content: %s] successfully", topic_name.c_str(), content.c_str());
}

void on_history(const std::string& topic_name, int64_t start_time, int32_t count)
{
    LOG_INFO("Fetching history of topic: %s successfully, data will come very soon.", topic_name.c_str());
}

int main(int argc, char** argv)
{
    pan::net::pubsub::client client("localhost", 8890);

    client.publish("Test", "This is a test 1", std::bind(on_published, std::placeholders::_1, std::placeholders::_2));
    client.publish("Test", "This is a test 2", std::bind(on_published, std::placeholders::_1, std::placeholders::_2));
    client.publish("Test", "This is a test 3", std::bind(on_published, std::placeholders::_1, std::placeholders::_2));
    client.publish("Test", "This is a test 4", std::bind(on_published, std::placeholders::_1, std::placeholders::_2));
    client.publish("Test", "This is a test 5");
    client.publish("Test", "This is a test 6");
    client.publish("Test", "This is a test 7");
    client.publish("Test", "This is a test 8");

    client.history("Test", 0, 1, std::bind(on_history, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}