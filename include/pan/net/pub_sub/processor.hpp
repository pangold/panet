#ifndef __PAN_NET_PUBSUB_PROCESSOR_HPP__
#define __PAN_NET_PUBSUB_PROCESSOR_HPP__

#include <memory>
#include <string>
#include <vector>
#include <pan/net/protocol.hpp>
#include <pan/net/protobuf/codec.hpp>

namespace pan { namespace net { namespace pubsub {

template <typename Session>
class processor_base {
public:
    typedef Session session_type;
    typedef typename session_type::pointer session_ptr;
    typedef typename session_type::weak session_weak;
    typedef protobuf::codec<session_type> codec_type;
    typedef std::map<std::string, session_ptr> pool_type;
    typedef std::map<std::string, std::map<std::string, session_ptr>> subscriber_map;

    processor_base(const std::string& name, pool_type& pool, codec_type& codec, subscriber_map& subs)
        : name_(name) 
        , pool_(pool)
        , codec_(codec) 
        , subscribers_(subs)
    { }

    std::string name() const noexcept { return name_; }
    pool_type& pool() noexcept { return pool_; }
    codec_type& codec() noexcept { return codec_; }
    subscriber_map& subscribers() noexcept { return subscribers_; }
    
protected:
    std::string name_;
    pool_type& pool_;
    codec_type& codec_;
    subscriber_map& subscribers_;
};

template <typename Session,
    typename Message>
class processor : public processor_base<Session> {
    typedef processor_base<Session> _Mybase;
public:
    typedef Message message_type;
    typedef std::shared_ptr<message_type> message_ptr;

    processor(pool_type& pool, codec_type& codec, subscriber_map& subs)
        : _Mybase("null", pool, codec, subs)
    {
        LOG_ERROR("unknown protobuf message");
    }
};

}}}

#endif // __PAN_NET_PUBSUB_PROCESSOR_HPP__