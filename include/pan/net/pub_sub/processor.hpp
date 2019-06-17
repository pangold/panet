#ifndef __PAN_NET_PUBSUB_PROCESSOR_HPP__
#define __PAN_NET_PUBSUB_PROCESSOR_HPP__

#include <memory>
#include <string>
#include <vector>
#include <pan/net/protocol.hpp>

namespace pan { namespace net { namespace pubsub {

template <typename Session>
class processor_base {
public:
    typedef Session session_type;
    typedef typename session_type::pointer session_ptr;
    typedef typename session_type::weak session_weak;
    typedef protocol::codec<session_type> codec_type;
    typedef std::map<std::string, session_ptr> session_map;
    typedef std::map<std::string, session_map> subscriber_map;

    processor_base(const std::string& name, codec_type& codec, subscriber_map& subs)
        : name_(name) 
        , codec_(codec) 
        , subscribers_(subs)
    { }

    std::string name() const noexcept { return name_; }
    codec_type& codec() noexcept { return codec_; }
    subscriber_map& subscribers() noexcept { return subscribers_; }
    
protected:
    std::string name_;
    codec_type& codec_;
    subscriber_map& subscribers_;
};

}}}

#endif // __PAN_NET_PUBSUB_PROCESSOR_HPP__