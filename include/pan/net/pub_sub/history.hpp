#ifndef __PAN_NET_PUBSUB_HISTORY_HPP__
#define __PAN_NET_PUBSUB_HISTORY_HPP__

#include <pan/base.hpp>
#include <pan/net/protobuf.hpp>
#include <pan/net/pub_sub/processor.hpp>

namespace pan { namespace net { namespace pubsub {

template <typename Session>
class processor<Session, Pango::PubSub::History> : public processor_base<Session> {
    typedef processor_base<Session> _Mybase;
public:
    typedef Pango::PubSub::History message_type;
    typedef std::shared_ptr<message_type> message_ptr;

    processor(pool_type& pool, codec_type& codec)
        : _Mybase("Pango.PubSub.History", pool, codec)
    {
        using namespace std::placeholders;
        auto cb = std::bind(&processor::on_message, this, _1, _2);
        codec.register_callback<message_type>(cb);
    }

    // really think about it?
    void history(const std::string& topic, int64_t before_time, int count)
    {

    }

private:
    void on_message(session_ptr session, message_ptr message)
    {

    }
};

}}}

#endif // __PAN_NET_PUBSUB_HISTORY_HPP__