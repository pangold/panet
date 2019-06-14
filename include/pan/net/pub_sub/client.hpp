#ifndef __PAN_NET_PUBSUB_CLIENT_HPP__
#define __PAN_NET_PUBSUB_CLIENT_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/pub_sub/handler.hpp>
#include <pan/net/pub_sub/storage_base.hpp>

namespace pan { namespace net { namespace pubsub {

class client : public tcp::client<pubsub::handler<storage_base>> {
public:
    typedef typename handler_type::topic_callback_type topic_callback_type;

    client(const std::string& host, uint16_t port)
        : tcp::client<pubsub::handler<storage_base>>(host, port)
    {

    }

    void publish(const std::string& topic, const std::string& content)
    {
        wait_for_session();
        // reset reply_callback
        // reply callback must be at the front of 'publish'(the same to the rest)
        handler_.publish_reply_callback(std::function<void(const std::string&, const std::string&)>());
        handler_.publish(topic, content);
    }

    void publish(const std::string& topic, const std::string& content, std::function<void(const std::string&, const std::string&)> cb)
    {
        wait_for_session();
        // reply callback must be at the front of 'publish'(the same to the rest)
        handler_.publish_reply_callback(std::move(cb));
        handler_.publish(topic, content);
    }

    void subscribe(const std::string& topic)
    {
        wait_for_session();
        handler_.subscribe_reply_callback(std::function<void(const std::string&)>());
        handler_.subscribe(topic);
    }

    void subscribe(const std::string& topic, std::function<void(const std::string&)> cb)
    {
        wait_for_session();
        handler_.subscribe_reply_callback(std::move(cb));
        handler_.subscribe(topic);
    }

    void cancel(const std::string& topic)
    {
        wait_for_session();
        handler_.cancel_reply_callback(std::function<void(const std::string&)>());
        handler_.cancel(topic);
    }

    void cancel(const std::string& topic, std::function<void(const std::string&)> cb)
    {
        wait_for_session();
        handler_.cancel_reply_callback(std::move(cb));
        handler_.cancel(topic);
    }

    // FIXME: the way(with params) to request history records is inconvenient
    // try to figure out a more convenient / reasonable way.
    void history(const std::string& topic, int64_t start_time, int32_t count)
    {
        wait_for_session();
        handler_.history_reply_callback(std::function<void(const std::string&, int64_t, int32_t)>());
        handler_.history(topic, start_time, count);
    }

    // FIXME: the way(with params) to request history records is inconvenient
    // try to figure out a more convenient / reasonable way.
    void history(const std::string& topic, int64_t start_time, int32_t count, std::function<void(const std::string&, int64_t, int32_t)> cb)
    {
        wait_for_session();
        handler_.history_reply_callback(std::move(cb));
        handler_.history(topic, start_time, count);
    }

    void register_topic_callback(topic_callback_type cb)
    {
        wait_for_session();
        handler_.register_topic_callback(std::move(cb));
    }
};

}}}

#endif // __PAN_NET_PUBSUB_CLIENT_HPP__