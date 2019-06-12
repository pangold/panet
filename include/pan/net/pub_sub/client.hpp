#ifndef __PAN_NET_PUBSUB_CLIENT_HPP__
#define __PAN_NET_PUBSUB_CLIENT_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/pub_sub/handler.hpp>

namespace pan { namespace net { namespace pubsub {

template <typename Storage>
class client : public tcp::client<pubsub::handler<Storage>> {
public:
    typedef typename handler_type::topic_callback_type topic_callback_type;

    client(const std::string& host, uint16_t port)
        : tcp::client<handler<Storage>>(host, port)
    {

    }

    void publish(const std::string& topic, const std::string& content)
    {
        wait_for_session();
        handler_.publish(topic, content);
    }

    void subscribe(const std::string& topic)
    {
        wait_for_session();
        handler_.subscribe(topic);
    }

    void cancel(const std::string& topic)
    {
        wait_for_session();
        handler_.cancel(topic);
    }

    void register_topic_callback(topic_callback_type cb)
    {
        wait_for_session();
        handler_.register_topic_callback(std::move(cb));
    }
};

}}}

#endif // __PAN_NET_PUBSUB_CLIENT_HPP__