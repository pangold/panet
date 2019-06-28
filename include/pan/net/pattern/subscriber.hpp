#ifndef __PAN_NET_PATTERN_SUBSCRIBER_HPP__
#define __PAN_NET_PATTERN_SUBSCRIBER_HPP__

#include <pan/net/pattern/pattern.pb.h>
#include <pan/net/pattern/requester.hpp>

namespace pan { namespace net { namespace pattern {

template <typename Session>
class subscriber {
    typedef Pango::PubSub message_type;
    typedef Pango::Topic topic_type;
    typedef std::shared_ptr<message_type> message_ptr;
    typedef std::shared_ptr<topic_type> topic_ptr;

    typedef Session session_type;
    typedef typename session_type::pointer session_ptr;
    typedef protocol::codec<session_type> codec_type;

public:
    // brief: callback content of subscribed topics
    // @param 1: topic name
    // @param 2: topic content
    typedef std::function<void(const std::string&, const std::string&)> topic_callback_type;

    // brief: constructor
    // @codec: protocol codec
    explicit subscriber(codec_type& codec)
        : codec_(codec)
        , requester_(codec)
    {
        using namespace std::placeholders;
        requester_.register_request_callback(std::bind(&subscriber::on_message, this, _1, _2, _3));
        codec.register_callback<topic_type>(std::bind(&subscriber::on_topic, this, _1, _2));
    }

    // brief: callback the topic data(could be both client and server)
    // @param cb: a callback function
    void register_topic_callback(topic_callback_type cb)
    {
        topic_callback_ = std::move(cb);
    }

    // brief: subscribe a specific topic asynchronously
    // @param session: the target session
    // @param topic: the topic name you expect to subscribe.
    // @param cb: callback of result state(only code is available)
    //        void(bool success, const std::string& topic)
    void subscribe(session_ptr session, const std::string& topic, 
        std::function<void(bool, const std::string&)> cb)
    {
        message_type sub;
        sub.set_type(message_type::SUBSCRIBE);
        sub.set_topic(topic);
        std::string request;
        if (sub.SerializeToString(&request)) {
            using namespace std::placeholders;
            auto cb2 = std::bind(&subscriber::on_reply_1, this, _1, _2, topic, std::move(cb));
            requester_.async_request(session, request, std::move(cb2));
        }
    }

    // brief: unsubscribe a specific topic asynchronously
    // @param session: the target session
    // @param topic: the topic name you expect to unsubscribe.
    // @param cb: callback of result state(only code is available)
    //        void(bool success, const std::string& topic)
    void unsubscribe(session_ptr session, const std::string& topic,
        std::function<void(bool, const std::string&)> cb)
    {
        message_type sub;
        sub.set_type(message_type::UNSUBSCRIBE);
        sub.set_topic(topic);
        std::string request;
        if (sub.SerializeToString(&request)) {
            using namespace std::placeholders;
            auto cb2 = std::bind(&subscriber::on_reply_1, this, _1, _2, topic, std::move(cb));
            requester_.async_request(session, request, std::move(cb2));
        }
    }

    // brief: publish a topic asynchronously
    // @param session: the target session
    // @param topic: the topic name you expect to publish.
    // @param content: the content of a topic you expect to publish.
    // @param cb: callback of result state(only code is available)
    //        void(bool success, const std::string& topic, const std::string& content)
    void publish(session_ptr session, const std::string& topic, const std::string& content,
        std::function<void(bool, const std::string&, const std::string&)> cb)
    {
        message_type pub;
        pub.set_type(message_type::PUBLISH);
        pub.set_topic(topic);
        pub.set_content(content);
        std::string request;
        if (pub.SerializeToString(&request)) {
            using namespace std::placeholders;
            auto cb2 = std::bind(&subscriber::on_reply_2, this, _1, _2, topic, content, std::move(cb));
            requester_.async_request(session, request, std::move(cb2));
        }
    }

private:
    // param topic: is not const or ref, 
    // callback will not be invoked immediately, 
    // and topic will be destroied and become invalid
    // so... it needs to copy(the same to on_publish_callback)
    // template <typename Callback>
    void on_reply_1(int code, const std::string& respond, std::string topic, 
        std::function<void(bool, const std::string&)> cb)
    {
        if (cb) cb(code == Pango::ReplyCode::SUCCESS, topic);
    }

    void on_reply_2(int code, const std::string& respond, std::string topic, std::string content,
        std::function<void(bool, const std::string&, const std::string&)> cb)
    {
        if (cb) cb(code == Pango::ReplyCode::SUCCESS, topic, content);
    }

    void on_topic(session_ptr session, topic_ptr message)
    {
        if (topic_callback_) topic_callback_(message->topic(), message->content());
    }

    int on_message(session_ptr session, const std::string& request, std::string&)
    {
        // codec to protobuf message Subscribe from string 'request'
        message_type message;
        if (!message.ParseFromString(request)) {
            LOG_ERROR("subscriber.parse.error: %s", request.c_str());
            return Pango::ReplyCode::FAILURE;
        }
        on_process(session, message);
        return Pango::ReplyCode::SUCCESS;
    }

    void on_process(session_ptr session, const message_type& message)
    {
        switch (message.type()) {
        case message_type::SUBSCRIBE:
            on_subscribe(session, message);
            break;
        case message_type::UNSUBSCRIBE:
            on_unsubscribe(session, message);
            break;
        case message_type::PUBLISH:
            on_publish(session, message);
            break;
        }
    }

    void on_subscribe(session_ptr session, const message_type& message)
    {
        subscribe_pool_[message.topic()][session->to_string()] = session;
    }

    void on_unsubscribe(session_ptr session, const message_type& message)
    {
        subscribe_pool_[message.topic()].erase(session->to_string());
    }

    void on_publish(session_ptr session, const message_type& message)
    {
        auto topic = std::make_shared<topic_type>();
        topic->set_topic(message.topic());
        topic->set_content(message.content());
        auto it = subscribe_pool_.find(message.topic());
        if (it != subscribe_pool_.end()) {
            for (auto& sub : it->second) {
                codec_.send(sub.second, topic);
            }
        }
    }

private:
    // codec protobuf message Pango::Request & Pango::Topic
    codec_type& codec_;
    // common request logic for publishing/subscribing/unsubscribing.
    requester<session_type> requester_;
    // callback topic
    topic_callback_type topic_callback_;
    // record topics that have been subscribed by which sessions
    std::map<std::string, std::map<std::string, session_ptr>> subscribe_pool_;

};

}}}

#endif // __PAN_NET_PATTERN_SUBSCRIBER_HPP__