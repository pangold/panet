#ifndef __PAN_NET_PATTERN_HANDLER_HPP__
#define __PAN_NET_PATTERN_HANDLER_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/pattern/requester.hpp>
#include <pan/net/pattern/subscriber.hpp>

namespace pan { namespace net { namespace pattern {

class handler : public tcp::handler_base<handler> {
    friend class session_type;
    typedef protocol::codec<session_type> codec_type;

public:
    // brief: contructor
    // @param pool: current connected session pool
    explicit handler(session_pool_type& pool)
        : tcp::handler_base<handler>(pool)
        , codec_()
        , requester_(codec_)
        , subscriber_(codec_)
    {

    }

    // brief: callback the topic data(could be both client and server)
    // @param cb: a callback function
    template <typename Callback>
    void register_topic_callback(Callback cb)
    {
        subscriber_.register_topic_callback(std::move(cb));
    }

    // brief: subscribe a specific topic asynchronously
    // @param session: the target session
    // @param topic: the topic name you expect to subscribe.
    // @param cb: callback of result state(only code is available)
    template <typename Callback>
    bool subscribe(const std::string& topic, Callback cb)
    {
        if (!session_) {
            LOG_ERROR("pattern.handler.subscribe.error: session is not ready yet.");
            return false;
        }
        subscriber_.subscribe(session_, topic, std::move(cb));
        return true;
    }

    // brief: unsubscribe a specific topic asynchronously
    // @param session: the target session
    // @param topic: the topic name you expect to unsubscribe.
    // @param cb: callback of result state(only code is available)
    template <typename Callback>
    bool unsubscribe(const std::string& topic, Callback cb)
    {
        if (!session_) {
            LOG_ERROR("pattern.handler.subscribe.error: session is not ready yet.");
            return false;
        }
        subscriber_.unsubscribe(session_, topic, std::move(cb));
        return true;
    }

    // brief: publish a topic asynchronously
    // @param session: the target session
    // @param topic: the topic name you expect to publish.
    // @param content: the content of a topic you expect to publish.
    // @param cb: callback of result state(only code is available)
    template <typename Callback>
    bool publish(const std::string& topic, const std::string& content, Callback cb)
    {
        if (!session_) {
            LOG_ERROR("pattern.handler.subscribe.error: session is not ready yet.");
            return false;
        }
        subscriber_.publish(session_, topic, content, std::move(cb));
        return true;
    }

protected:
    void on_session_start(session_ptr session) override
    {
        session_ = session;
    }

    void on_session_stop(session_ptr session) override
    {
        session_.reset();
    }

    size_t on_message(session_ptr session, const void* data, size_t size) override
    {
        // protocol::codec has responsibilities to dispatch
        return codec_.on_message(session, data, size);
    }

protected:
    codec_type codec_;
    requester<session_type> requester_;
    subscriber<session_type> subscriber_;
    session_ptr session_;

};

}}}

#endif // __PAN_NET_PATTERN_HANDLER_HPP__