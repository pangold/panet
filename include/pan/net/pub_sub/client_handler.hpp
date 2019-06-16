#ifndef __PAN_NET_PUBSUB_CLIENT_HANDLER_HPP__
#define __PAN_NET_PUBSUB_CLIENT_HANDLER_HPP__

#include <memory>
#include <functional>
#include <pan/net/protocol.hpp>
#include <pan/net/protobuf.hpp>
#include <pan/net/pub_sub/pubsub.pb.h>
#include <pan/net/pub_sub/storage_base.hpp>
#include <pan/net/pub_sub/canceler.hpp>
#include <pan/net/pub_sub/historian.hpp>
#include <pan/net/pub_sub/notifier.hpp>
#include <pan/net/pub_sub/publisher.hpp>
#include <pan/net/pub_sub/subscriber.hpp>

namespace pan { namespace net { namespace pubsub {

class client_handler : public protocol::datagram_handler_base<client_handler> {
    friend class session_type;
    typedef protobuf::codec<session_type> codec_type;
    typedef canceler<session_type> canceler_type;
    typedef historian<session_type> historian_type;
    typedef notifier<session_type> notifier_type;
    typedef publisher<session_type> publisher_type;
    typedef subscriber<session_type> subscriber_type;
    typedef subscriber_type::subscriber_map subscriber_map;
    typedef historian_type::topic_list topic_list;
public:
    typedef notifier_type::topic_callback_type topic_callback_type;
    
    explicit client_handler(session_pool_type& pool)
        : protocol::datagram_handler_base<client_handler>(pool)
        , codec_()
        , subscribers_()
        , canceler_(codec_, subscribers_)
        , historian_(codec_, subscribers_)
        , notifier_(codec_, subscribers_)
        , publisher_(codec_, subscribers_)
        , subscriber_(codec_, subscribers_)
    {

    }

    // publish a new topic
    void publish(const std::string& topic, const std::string& content)
    {
        publisher_.publish(topic, content);
    }

    // for a client, he shouldn't know the detail of protocol(here's protobuf message). 
    // so just reply with info that he inputed.
    // FIXME: callback should be with state field to show success or not.
    // request success?
    void publish_reply_callback(std::function<void(const std::string&, const std::string&)> cb)
    {
        publisher_.register_reply_callback(std::move(cb));
    }

    // subscribe a topic
    void subscribe(const std::string& topic)
    {
        subscriber_.subscribe(topic);
    }

    // FIXME: callback should be with state field to show success or not.
    // request success?
    void subscribe_reply_callback(std::function<void(const std::string&)> cb)
    {
        subscriber_.register_reply_callback(std::move(cb));
    }

    // unsubscribe a topic
    void cancel(const std::string& topic)
    {
        canceler_.cancel(topic);
    }

    // FIXME: callback should be with state field to show success or not.
    // request success?
    void cancel_reply_callback(std::function<void(const std::string&)> cb)
    {
        canceler_.register_reply_callback(std::move(cb));
    }

    // request history records of specific topic.
    // FIXME: the way(with params) to request history records is inconvenient
    // try to figure out a more convenient / reasonable way.
    void history(const std::string& topic, int64_t start_time, int32_t count)
    {
        historian_.history(topic, start_time, count);
    }

    // FIXME: callback should be with state field to show success or not.
    // request success?
    void history_reply_callback(std::function<void(const std::string&, int64_t, int32_t)> cb)
    {
        historian_.register_reply_callback(std::move(cb));
    }

    // topic callback, only subscribed sessions(users) 
    // will received topic info through cb.
    void register_topic_callback(topic_callback_type cb)
    {
        notifier_.register_topic_callback(std::move(cb));
    }
    
protected:
    void on_session_start(session_ptr session)
    {
        historian_.set_session(session);
        publisher_.set_session(session);
        subscriber_.set_session(session);
        canceler_.set_session(session);
    }

    void on_session_stop(session_ptr)
    {
        historian_.set_session(nullptr);
        publisher_.set_session(nullptr);
        subscriber_.set_session(nullptr);
        canceler_.set_session(nullptr);
    }

    void on_datagram(session_ptr session, datagram_ptr datagram)
    {
        // protobuf::codec is responsible to dispatch protobuf message 
        // to cancel/history/notify/publish/subscribe message.
        codec_.on_message(session, *datagram);
    }
    
protected:
    codec_type codec_;
    subscriber_map subscribers_;
    canceler_type canceler_;
    historian_type historian_;
    notifier_type notifier_;
    publisher_type publisher_;
    subscriber_type subscriber_;

};

}}}

#endif // __PAN_NET_PUBSUB_CLIENT_HANDLER_HPP__