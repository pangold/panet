#ifndef __PAN_NET_RPC_PROCESSOR_HPP__
#define __PAN_NET_RPC_PROCESSOR_HPP__

#include <memory>
#include <string>
#include <vector>
#include <pan/net/protocol.hpp>
#include <pan/net/protobuf/codec.hpp>

namespace pan { namespace net { namespace rpc {

template <typename Session>
class processor_base {
public:
    typedef Session session_type;
    typedef typename session_type::pointer session_ptr;
    typedef std::map<std::string, session_ptr> pool_type;
    typedef protobuf::codec<session_type> codec_type;

    processor_base(const std::string& name, pool_type& pool, codec_type& codec)
        : name_(name) 
        , pool_(pool)
        , codec_(codec) 
    { }

    std::string name() const noexcept { return name_; }
    pool_type& pool() { return pool_; }
    codec_type& codec() { return codec_; }
protected:
    std::string name_;
    pool_type& pool_;
    codec_type& codec_;
};

template <typename Session,
    typename Message>
class processor : public processor_base<Session> {
    typedef processor_base<Session> _Mybase;
public:
    typedef Message message_type;
    typedef std::shared_ptr<message_type> message_ptr;

    processor(pool_type& pool, codec_type& codec)
        : _Mybase("null", pool, codec)
    {
        LOG_ERROR("unknown protobuf message");
    }
};

}}}

#endif // __PAN_NET_RPC_PROCESSOR_HPP__