#ifndef __PAN_NET_PUBSUB_PUBLISH_HPP__
#define __PAN_NET_PUBSUB_PUBLISH_HPP__

#include <pan/base.hpp>
#include <pan/net/protobuf.hpp>
#include <pan/net/pub_sub/processor.hpp>

namespace pan { namespace net { namespace pubsub {

template <typename Session>
class processor<Session, Pango::PubSub::Publish> : public processor_base<Session> {
    typedef processor_base<Session> _Mybase;
public:
    typedef Pango::PubSub::Publish message_type;
    typedef std::shared_ptr<message_type> message_ptr;
    // FIXME: let all of subscriber_map together.
    typedef std::map<std::string, std::list<session_ptr>> subscriber_map;

    processor(pool_type& pool, codec_type& codec, subscriber_map& subscribers)
        : _Mybase("Pango.PubSub.Publish", pool, codec)
        , subscribers_(subscribers)
    {
        using namespace std::placeholders;
        auto cb = std::bind(&processor::on_message, this, _1, _2);
        codec.register_callback<message_type>(cb);
    }

    // for client
    void publish(const std::string& name, const std::string& content)
    {
        assert(!pool().empty());
        auto& session = pool().begin()->second;
        auto message = std::make_shared<message_type>();
        auto topic = message->mutable_topic();
        topic->set_name(name);
        topic->set_content(content);
        // no datetime, but at server's side, 
        // because server side's clock is more reliable.
        // no number(id), but at server's side too.
        codec().send(session, message);
        // TODO: confirm list
    }

private:
    void on_message(session_ptr session, message_ptr message)
    {
        if (message->has_reply()) {
            on_reply(session, message);
        } else {
            on_publish(session, message);
        }
    }

    // reply to client
    void on_reply(session_ptr session, message_ptr message)
    {
        // TODO: confirm
    }

    // request to server.
    void on_publish(session_ptr session, message_ptr message)
    {
        auto subscribers = subscribers_[message->topic().name()];
        auto topic = std::make_shared<Pango::PubSub::Topic>(message->topic());
        // 
        topic->set_who(session->to_string());
        // FIXME: make number(id) sense
        topic->set_number(1);
        // datetime's microseconds
        topic->set_datetime(std::chrono::steady_clock().now().time_since_epoch().count() / 1000);
        for (auto e : subscribers) {
            codec().send(e, topic);
        }
        reply(session);
        // TODO: storage.
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

#endif // __PAN_NET_PUBSUB_PUBLISH_HPP__