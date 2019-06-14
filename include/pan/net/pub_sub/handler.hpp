#ifndef __PAN_NET_PUBSUB_HANDLER_HPP__
#define __PAN_NET_PUBSUB_HANDLER_HPP__

#include <memory>
#include <functional>
#include <pan/net/protocol.hpp>
#include <pan/net/protobuf.hpp>
#include <pan/net/pub_sub/pubsub.pb.h>
#include <pan/net/pub_sub/storage_base.hpp>
#include <pan/net/pub_sub/cancel.hpp>
#include <pan/net/pub_sub/history.hpp>
#include <pan/net/pub_sub/notify.hpp>
#include <pan/net/pub_sub/publish.hpp>
#include <pan/net/pub_sub/subscribe.hpp>

namespace pan { namespace net { namespace pubsub {

template <typename Storage>
class handler : public protocol::datagram_handler_base<handler<Storage>> {
    typedef protobuf::codec<session_type> codec_type;
    typedef processor<session_type, Pango::PubSub::Cancel> cancel_type;
    typedef processor<session_type, Pango::PubSub::History> history_type;
    typedef processor<session_type, Pango::PubSub::Topic> notify_type;
    typedef processor<session_type, Pango::PubSub::Publish> publish_type;
    typedef processor<session_type, Pango::PubSub::Subscribe> subscribe_type;
    typedef typename subscribe_type::subscriber_map subscriber_map;
    typedef typename history_type::topic_list topic_list;
public:
    typedef Storage storage_type;
    typedef typename notify_type::topic_callback_type topic_callback_type;
    
    handler() 
        : codec_()
        , subscribers_()
        , storage_()
        , cancel_(pool_, codec_, subscribers_)
        , history_(pool_, codec_, subscribers_)
        , notify_(pool_, codec_, subscribers_)
        , publish_(pool_, codec_, subscribers_)
        , subscribe_(pool_, codec_, subscribers_)
    {
        using namespace std::placeholders;
        subscribe_.register_complete_callback(std::bind(&handler::on_subscribe, this, _1, _2));
        cancel_.register_complete_callback(std::bind(&handler::on_cancel, this, _1, _2));
        publish_.register_complete_callback(std::bind(&handler::on_publish, this, _1, _2));
        history_.set_history_operation(std::bind(&handler::on_history, this, _1, _2, _3));
    }

    // publish a new topic
    void publish(const std::string& topic, const std::string& content)
    {
        publish_.publish(topic, content);
    }

    // for a client, he shouldn't know the detail of protocol(here's protobuf message). 
    // so just reply with info that he inputed.
    // FIXME: callback should be with state field to show success or not.
    // request success?
    void publish_reply_callback(std::function<void(const std::string&, const std::string&)> cb)
    {
        publish_.register_reply_callback(std::move(cb));
    }

    // subscribe a topic
    void subscribe(const std::string& topic)
    {
        subscribe_.subscribe(topic);
    }

    // FIXME: callback should be with state field to show success or not.
    // request success?
    void subscribe_reply_callback(std::function<void(const std::string&)> cb)
    {
        subscribe_.register_reply_callback(std::move(cb));
    }

    // unsubscribe a topic
    void cancel(const std::string& topic)
    {
        cancel_.cancel(topic);
    }

    // FIXME: callback should be with state field to show success or not.
    // request success?
    void cancel_reply_callback(std::function<void(const std::string&)> cb)
    {
        cancel_.register_reply_callback(std::move(cb));
    }

    // request history records of specific topic.
    // FIXME: the way(with params) to request history records is inconvenient
    // try to figure out a more convenient / reasonable way.
    void history(const std::string& topic, int64_t start_time, int32_t count)
    {
        history_.history(topic, start_time, count);
    }

    // FIXME: callback should be with state field to show success or not.
    // request success?
    void history_reply_callback(std::function<void(const std::string&, int64_t, int32_t)> cb)
    {
        history_.register_reply_callback(std::move(cb));
    }

    // topic callback, only subscribed sessions(users) 
    // will received topic info through cb.
    void register_topic_callback(topic_callback_type cb)
    {
        notify_.register_topic_callback(std::move(cb));
    }

    // Load previous subscribed info from elsewhere(such DB), 
    // Storage module is an extension module that to implement variable things.
    // Example: I login a system that based on this pub_sub, and I subscribed a topic
    //          But next day, I login again, do I need to subscribe topics again???
    //          Or just load the previous subscribed info from storage module???
    void load()
    {
        storage_.load();
    }
    
protected:
    void on_extra_session_stop(session_ptr session)
    {
        for (auto& e : subscribers_) {
            auto it = e.second.find(session->to_string());
            if (it != e.second.end()) {
                e.second.erase(it);
            }
        }
    }

    // FIXME: when session just start, context won't be available at that moment...
    void on_extra_session_start(session_ptr session)
    {
        // FIXME: context should be session::context()
        std::string context = session->to_string();
        std::vector<std::string> topics;
        storage_.subscribed_topics(context, topics);
        for (auto& topic : topics) {
            subscribers_[topic][session->to_string()] = session;
        }
    }

    void on_subscribe(session_ptr session, std::shared_ptr<Pango::PubSub::Subscribe> message)
    {
        // FIXME: context should be session::context()
        std::string context = session->to_string();
        storage_.add_subscriber(context, message->topic());
    }

    void on_cancel(session_ptr session, std::shared_ptr<Pango::PubSub::Cancel> message)
    {
        // FIXME: context should be session::context()
        std::string context = session->to_string();
        storage_.remove_subscriber(context, message->topic());
    }

    void on_publish(session_ptr session, std::shared_ptr<Pango::PubSub::Publish> message)
    {
        // FIXME: context should be session::context()
        std::string context = session->to_string();
        storage_.add_topic(context, message->topic());
    }

    void on_history(session_ptr session, std::shared_ptr<Pango::PubSub::History> message, topic_list& topics)
    {
        // FIXME: context should be session::context()
        std::string context = session->to_string();
        return storage_.find_topic(context, message->topic(), message->start_time(), message->count(), topics);
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
    storage_type storage_;
    cancel_type cancel_;
    history_type history_;
    notify_type notify_;
    publish_type publish_;
    subscribe_type subscribe_;

};

}}}

#endif // __PAN_NET_PUBSUB_HANDLER_HPP__