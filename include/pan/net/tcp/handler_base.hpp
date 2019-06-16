#ifndef __PAN_NET_TCP_HANDLER_BASE_HPP__
#define __PAN_NET_TCP_HANDLER_BASE_HPP__

#include <pan/net/tcp/session.hpp>
#include <pan/net/tcp/session_pool.hpp>

namespace pan { namespace net { namespace tcp {

template <typename Inherit>
class handler_base {
protected:
    template <typename Handler> friend class session;
public:
    typedef session<Inherit> session_type;
    typedef session_pool<Inherit> session_pool_type;
    typedef typename session_type::pointer session_ptr;
    typedef typename session_type::weak session_weak;

    explicit handler_base(session_pool_type& pool)
        : pool_(pool)
    { }

    virtual ~handler_base() { }

protected:
    // offer the reference of session pool
    session_pool_type& pool() { return pool_; }

    // an extension for session start
    virtual void on_session_start(session_ptr) { }

    // an extension for session stop
    virtual void on_session_stop(session_ptr) { }

    // for data statistics
    virtual void on_write(session_ptr, const void*, size_t) { }

    // for data statistics and real business logic processing
    virtual size_t on_message(session_ptr, const void*, size_t) = 0;

protected:
    session_pool_type& pool_;

};

}}}

#endif // __PAN_NET_TCP_HANDLER_BASE_HPP__
