#ifndef __PAN_NET_TCP_HANDLER_BASE_HPP__
#define __PAN_NET_TCP_HANDLER_BASE_HPP__

#include <memory>
#include <string>
#include <pan/base.hpp>
#include <pan/net/tcp/session.hpp>

namespace pan { namespace net { namespace tcp {

class handler_base {
public:
    typedef session session_type;
    typedef session_type::key_type key_type;
    typedef session_type::pointer session_ptr;
    typedef pan::map<key_type, session_ptr> pool_type;

    virtual ~handler_base() { }
    
    // to manage connections
    void on_session_start(session_ptr session) 
    {
        pool_.insert(session->id(), session);
    }
    
    // to manage connections
    void on_session_stop(session_ptr session) 
    {
        pool_.remove(session->id());
    }
    
    // for data statistics
    virtual void on_write(session_ptr session, const void* data, std::size_t size) 
    {
        
    }
    
    // for data statistics and real business logic processing
    virtual std::size_t on_message(session_ptr session, const void* data, std::size_t size) 
    { 
        return size; 
    }
    
protected:
    pool_type pool_;

};

}}}

#endif // __PAN_NET_TCP_HANDLER_BASE_HPP__
