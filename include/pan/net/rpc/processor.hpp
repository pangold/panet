#ifndef __PAN_NET_RPC_PROCESSOR_HPP__
#define __PAN_NET_RPC_PROCESSOR_HPP__

#include <memory>
#include <string>
#include <vector>
#include <pan/net/protocol.hpp>

namespace pan { namespace net { namespace rpc {

template <typename Session>
class processor_base {
public:
    typedef Session session_type;
    typedef typename session_type::pointer session_ptr;
    typedef protocol::codec<session_type> codec_type;

    processor_base(const std::string& name, codec_type& codec)
        : name_(name)
        , codec_(codec) 
    { }

    std::string name() const noexcept { return name_; }
    codec_type& codec() { return codec_; }

protected:
    std::string name_;
    codec_type& codec_;

};

}}}

#endif // __PAN_NET_RPC_PROCESSOR_HPP__