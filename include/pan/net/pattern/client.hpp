#ifndef __PAN_NET_PATTERN_CLIENT_HPP__
#define __PAN_NET_PATTERN_CLIENT_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/pattern/handler.hpp>

namespace pan { namespace net { namespace pattern {

class client : private tcp::client<handler> {
public:
    client(const std::string& host, uint16_t port)
        : tcp::client<handler>(host, port)
    {

    }

    // brief: callback the topic data(could be both client and server)
    // @param cb: a callback function
    template <typename Callback>
    void register_topic_callback(Callback cb)
    {
        handler_.register_topic_callback(std::move(cb));
    }

    // brief: subscribe a specific topic asynchronously
    // @param session: the target session
    // @param topic: the topic name you expect to subscribe.
    // @param cb: callback of result state(only code is available)
    template <typename Callback>
    bool subscribe(const std::string& topic, Callback cb)
    {
        return handler_.subscribe(topic, std::move(cb));
    }

    // brief: unsubscribe a specific topic asynchronously
    // @param session: the target session
    // @param topic: the topic name you expect to unsubscribe.
    // @param cb: callback of result state(only code is available)
    template <typename Callback>
    bool unsubscribe(const std::string& topic, Callback cb)
    {
        return handler_.unsubscribe(topic, std::move(cb));
    }

    // brief: publish a topic asynchronously
    // @param session: the target session
    // @param topic: the topic name you expect to publish.
    // @param content: the content of a topic you expect to publish.
    // @param cb: callback of result state(only code is available)
    template <typename Callback>
    bool publish(const std::string& topic, const std::string& content, Callback cb)
    {
        return handler_.publish(topic, content, std::move(cb));
    }
};

}}}

#endif // __PAN_NET_PATTERN_CLIENT_HPP__