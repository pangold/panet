#ifndef __PAN_NET_TCP_HANDLER_BASE_HPP__
#define __PAN_NET_TCP_HANDLER_BASE_HPP__

#include <memory>

namespace pan { namespace net { namespace tcp {

template <typename Handler> 
class session;

template <typename Inherit>
class handler_base {
public:
    typedef session<Inherit> session_type;
    typedef typename session_type::pointer session_ptr;

    virtual ~handler_base() { }
    // to manage connections
    virtual void on_session_start(session_ptr) { }
    // to manage connections
    virtual void on_session_stop(session_ptr) { }
    // for data statistics
    virtual void on_write(session_ptr, const void*, std::size_t) { }
    // for data statistics and real business logic processing
    virtual std::size_t on_message(session_ptr, const void*, std::size_t) = 0;
};

}}}

#endif // __PAN_NET_TCP_HANDLER_BASE_HPP__
