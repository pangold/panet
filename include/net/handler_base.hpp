#ifndef __PAN_NET_HANDLER_BASE_HPP__
#define __PAN_NET_HANDLER_BASE_HPP__

#include <memory>
#include <string>

namespace pan { namespace net { 

// template typename session is going to use for extension(handling connection for tcp, upd(an abstract connection)).
// now tcp is implemented, but upd will be implemented in the future.
template <typename Session>
class handler_base {
    friend typename Session;
public:
    using session_ptr = typename Session::pointer;
    virtual ~handler_base() { }

protected:
    virtual void on_start(session_ptr) { }
    virtual void on_stop(session_ptr) { }
    virtual void on_wrote(session_ptr, const void*, std::size_t) { }
    virtual std::size_t on_message(session_ptr, const void*, std::size_t size) { return size; }
    virtual void on_error(session_ptr, const std::string&) { }

};

}}

#endif // __PAN_NET_HANDLER_BASE_HPP__
