#ifndef __PAN_NET_PUBSUB_NOTIFY_HPP__
#define __PAN_NET_PUBSUB_NOTIFY_HPP__

#include <pan/base.hpp>
#include <pan/net/protobuf.hpp>
#include <pan/net/pub_sub/processor.hpp>

namespace pan { namespace net { namespace pubsub {

template <typename Session>
class processor<Session, Pango::PubSub::Topic> : public processor_base<Session> {
    typedef processor_base<Session> _Mybase;
public:
    typedef Pango::PubSub::Topic message_type;
    typedef std::shared_ptr<message_type> message_ptr;
    typedef std::function<void(message_ptr)> topic_callback_type;

    processor(pool_type& pool, codec_type& codec, subscriber_map& subs)
        : _Mybase("Pango.PubSub.Topic", pool, codec, subs)
    {
        using namespace std::placeholders;
        auto cb = std::bind(&processor::on_message, this, _1, _2);
        codec.register_callback<message_type>(cb);
    }

    void register_topic_callback(topic_callback_type cb)
    {
        topic_callback_ = std::move(cb);
    }

private:
    void on_message(session_ptr session, message_ptr message)
    {
        if (topic_callback_) topic_callback_(message);
    }

private:
    topic_callback_type topic_callback_;
};

}}}

#endif // __PAN_NET_PUBSUB_NOTIFY_HPP__