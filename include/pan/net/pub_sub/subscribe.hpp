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
    typedef std::function<void(session_ptr, message_ptr)> complete_callback_type;
    typedef std::function<void(const std::string&)> reply_callback_type;
    typedef std::tuple<message_ptr, reply_callback_type> reply_operation;

    processor(pool_type& pool, codec_type& codec, subscriber_map& subs)
        : _Mybase("Pango.PubSub.Subscribe", pool, codec, subs)
    {
        using namespace std::placeholders;
        auto cb = std::bind(&processor::on_message, this, _1, _2);
        codec.register_callback<message_type>(cb);
    }

    // for server side.
    // after processing sub request, you can do some extra thing, 
    // such as store it in DB through this callback
    void register_complete_callback(complete_callback_type cb)
    {
        complete_callback_ = std::move(cb);
    }

    // for client side.
    void register_reply_callback(reply_callback_type cb)
    {
        reply_callback_ = std::move(cb);
    }

    // for client side.
    void subscribe(const std::string& topic)
    {
        assert(!pool().empty());
        static int32_t number = 0;
        auto& session = pool().begin()->second;
        auto message = std::make_shared<message_type>();
        message->set_number(++number);
        message->set_topic(topic);
        codec().send(session, message);
        confirmations_[number] = std::make_tuple(message, reply_callback_);
        // TODO: post timer queue of io_services(event loop)
        // to handle timeout...
    }

private:
    // entrance of received data(callback from elsewhere), 
    // which had been decoded to specific protobuf message.
    void on_message(session_ptr session, message_ptr message)
    {
        if (message->has_reply()) {
            on_reply(session, message);
        } else {
            on_subscribe(session, message);
            reply(session, message);
        }
    }

    // reply from server
    void on_reply(session_ptr session, message_ptr message)
    {
        auto it = confirmations_.find(message->number());
        if (it != confirmations_.end()) {
            auto& callback = std::get<1>(it->second);
            if (callback) callback(message->topic());
            confirmations_.erase(message->number());
        }
    }

    // request to server
    void on_subscribe(session_ptr session, message_ptr message)
    {
        subscribers()[message->topic()][session->to_string()] = session;
        if (complete_callback_) complete_callback_(session, message);
    }

    // reply to client
    void reply(session_ptr session, message_ptr message)
    {
        message->set_reply(true);
        codec().send(session, message);
    }

private:
    std::map<int32_t, reply_operation> confirmations_;
    complete_callback_type complete_callback_;
    reply_callback_type reply_callback_;
};

}}}

#endif // __PAN_NET_PUBSUB_SUBSCRIBE_HPP__