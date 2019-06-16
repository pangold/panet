#ifndef __PAN_NET_PUBSUB_SERVER_HANDLER_HPP__
#define __PAN_NET_PUBSUB_SERVER_HANDLER_HPP__

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

template <typename Storage>
class server_handler : public protocol::datagram_handler_base<server_handler<Storage>> {
    friend class session_type;
    typedef protobuf::codec<session_type> codec_type;
    typedef canceler<session_type> canceler_type;
    typedef historian<session_type> historian_type;
    typedef notifier<session_type> notifier_type;
    typedef publisher<session_type> publisher_type;
    typedef subscriber<session_type> subscriber_type;
    typedef typename subscriber_type::subscriber_map subscriber_map;
    typedef typename historian_type::topic_list topic_list;
public:
    typedef Storage storage_type;
    
    explicit server_handler(session_pool_type& pool)
        : protocol::datagram_handler_base<server_handler<Storage>>(pool)
        , codec_()
        , subscribers_()
        , storage_()
        , canceler_(codec_, subscribers_)
        , historian_(codec_, subscribers_)
        , notifier_(codec_, subscribers_)
        , publisher_(codec_, subscribers_)
        , subscriber_(codec_, subscribers_)
    {
        using namespace std::placeholders;
        subscriber_.register_complete_callback(std::bind(&server_handler::on_subscribe, this, _1, _2));
        canceler_.register_complete_callback(std::bind(&server_handler::on_cancel, this, _1, _2));
        publisher_.register_complete_callback(std::bind(&server_handler::on_publish, this, _1, _2));
        historian_.set_history_operation(std::bind(&server_handler::on_history, this, _1, _2, _3));
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
    void on_session_start(session_ptr session)
    {
        // FIXME: context should be session::context()
        std::string context = session->to_string();
        std::vector<std::string> topics;
        storage_.subscribed_topics(context, topics);
        for (auto& topic : topics) {
            subscribers_[topic][session->to_string()] = session;
        }
    }

    void on_session_stop(session_ptr session)
    {
        for (auto& e : subscribers_) {
            auto it = e.second.find(session->to_string());
            if (it != e.second.end()) {
                e.second.erase(it);
            }
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
    canceler_type canceler_;
    historian_type historian_;
    notifier_type notifier_;
    publisher_type publisher_;
    subscriber_type subscriber_;

};

}}}

#endif // __PAN_NET_PUBSUB_SERVER_HANDLER_HPP__