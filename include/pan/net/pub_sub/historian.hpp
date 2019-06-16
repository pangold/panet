#ifndef __PAN_NET_PUBSUB_HISTORIAN_HPP__
#define __PAN_NET_PUBSUB_HISTORIAN_HPP__

#include <pan/base.hpp>
#include <pan/net/protobuf.hpp>
#include <pan/net/pub_sub/processor.hpp>

namespace pan { namespace net { namespace pubsub {

template <typename Session>
class historian : public processor_base<Session> {
public:
    typedef Pango::PubSub::History message_type;
    typedef Pango::PubSub::Topic topic_type;
    typedef std::shared_ptr<message_type> message_ptr;
    typedef std::shared_ptr<topic_type> topic_ptr;
    typedef std::vector<topic_ptr> topic_list;
    typedef std::function<void(session_ptr, message_ptr, topic_list&)> history_operation_type;
    typedef std::function<void(const std::string&, int64_t, int32_t)> reply_callback_type;
    typedef std::tuple<message_ptr, reply_callback_type> reply_operation;
    typedef std::map<int32_t, reply_operation> confirmation_map;

    historian(codec_type& codec, subscriber_map& subs)
        : processor_base<Session>("Pango.PubSub.History", codec, subs)
    {
        using namespace std::placeholders;
        auto cb = std::bind(&historian::on_message, this, _1, _2);
        codec.register_callback<message_type>(cb);
    }

    // for client side.
    void set_session(session_ptr session)
    {
        session_ = session;
    }

    // for server side.
    // after processing sub request, you can do some extra thing, 
    // such as store it in DB through this callback
    void set_history_operation(history_operation_type op)
    {
        history_operation_ = std::move(op);
    }

    // for client side.
    void register_reply_callback(reply_callback_type cb)
    {
        reply_callback_ = std::move(cb);
    }

    // for client side.
    void history(const std::string& topic, int64_t start_time, int count)
    {
        static int32_t number = 0;
        auto message = std::make_shared<message_type>();
        message->set_number(++number);
        message->set_topic(topic);
        message->set_start_time(start_time);
        message->set_count(count);
        if (!session_) {
            LOG_ERROR("pubsub.history: session is not ready yet");
            return;
        }
        codec().send(session_, message);
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
            on_history(session, message);
            reply(session, message);
        }
    }

    // reply from server
    void on_reply(session_ptr session, message_ptr message)
    {
        auto it = confirmations_.find(message->number());
        if (it != confirmations_.end()) {
            auto& callback = std::get<1>(it->second);
            if (callback) callback(message->topic(), message->start_time(), message->count());
            confirmations_.erase(message->number());
        }
    }

    // request to server
    void on_history(session_ptr session, message_ptr message)
    {
        if (!history_operation_) return;
        // fetch records from storage module, 
        // and then, send them back, just like action "publish".
        topic_list topics;
        history_operation_(session, message, topics);
        for (auto& topic : topics) {
            codec().send(session, topic);
        }
    }

    // reply to client
    void reply(session_ptr session, message_ptr message)
    {
        message->set_reply(true);
        codec().send(session, message);
    }

private:
    session_ptr session_;
    confirmation_map confirmations_;
    history_operation_type history_operation_;
    reply_callback_type reply_callback_;

};

}}}

#endif // __PAN_NET_PUBSUB_HISTORIAN_HPP__