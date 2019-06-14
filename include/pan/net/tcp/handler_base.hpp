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
    typedef typename session_type::weak session_weak;

    virtual ~handler_base() { }

    // to manage sessions
    // be invoked by async_accept/async_connect completion callback of io_context
    // in another word, by event loop of io_context
    void on_session_start(session_ptr session)
    {
        // TODO: optimize...
        std::lock_guard<std::mutex> lock(mutex_);
        pool_[session->to_string()] = session;
        on_extra_session_start(session);
    }

    // to manage sessions
    // be invoked by async_write/async_read completion(error) callback of io_context
    // by destructor of session and client.
    // if by destructor of session, it means no one hold this session anymore(unnormal)
    // if by destructor of client, it gets its job done and quit.
    // so it will be mainly invoked by event loop of io_context.
    // what if find/get and remove from pool at the same time in different threads?
    void on_session_stop(session_ptr session)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pool_.erase(session->to_string());
        on_extra_session_stop(session);
    }

    // an utility to get session
    session_ptr get(const std::string& ip)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return pool_[ip];
    }

    // an extension for session start
    virtual void on_extra_session_start(session_ptr session) { }

    // an extension for session stop
    virtual void on_extra_session_stop(session_ptr session) { }

    // for data statistics
    virtual void on_write(session_ptr, const void*, size_t) { }

    // for data statistics and real business logic processing
    virtual size_t on_message(session_ptr, const void*, size_t) = 0;

protected:
    // FIXME: shouldn't manage two session pools. One is perfect. 
    // Pool in handler? Or tcp::client&tcp::clients&tcp::server?
    std::map<std::string, session_ptr> pool_;
    std::mutex mutex_;

};

}}}

#endif // __PAN_NET_TCP_HANDLER_BASE_HPP__
