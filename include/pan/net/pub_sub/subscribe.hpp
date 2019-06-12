#ifndef __PAN_NET_PUBSUB_SUBSCRIBE_HPP__
#define __PAN_NET_PUBSUB_SUBSCRIBE_HPP__

#include <pan/base.hpp>
#include <pan/net/protobuf.hpp>
#include <pan/net/pub_sub/processor.hpp>

namespace pan { namespace net { namespace pubsub {

template <typename Session>
class processor<Session, Pango::PubSub::Subscribe> : public processor_base<Session> {
    typedef processor_base<Session> _Mybase;
public:
    typedef Pango::PubSub::Subscribe message_type;
    typedef std::shared_ptr<message_type> message_ptr;
    // FIXME: let all of subscriber_map together.
    typedef std::map<std::string, std::list<session_ptr>> subscriber_map;

    processor(pool_type& pool, codec_type& codec, subscriber_map& subscribers)
        : _Mybase("Pango.PubSub.Subscribe", pool, codec)
        , subscribers_(subscribers)
    {
        using namespace std::placeholders;
        auto cb = std::bind(&processor::on_message, this, _1, _2);
        codec.register_callback<message_type>(cb);
    }

    // for client
    void subscribe(const std::string& topic)
    {
        assert(!pool().empty());
        auto& session = pool().begin()->second;
        auto message = std::make_shared<message_type>();
        message->set_topic(topic);
        codec().send(session, message);
        // TODO: confirm list
    }

private:
    void on_message(session_ptr session, message_ptr message)
    {
        if (message->has_reply()) {
            on_reply(session, message);
        } else {
            on_subscribe(session, message);
        }
    }

    // reply to client
    void on_reply(session_ptr session, message_ptr message)
    {
        // TODO: confirm
    }

    // request to server.
    void on_subscribe(session_ptr session, message_ptr message)
    {
        subscribers_[message->topic()].push_back(session);
        reply(session);
    }

    void reply(session_ptr session)
    {
        auto message = std::make_shared<message_type>();
        message->set_reply(true);
        codec().send(session, message);
    }

private:
    subscriber_map& subscribers_;

};

}}}

#endif // __PAN_NET_PUBSUB_SUBSCRIBE_HPP__