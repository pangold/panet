#ifndef __PAN_NET_TCP_PROXY_HANDLER_BASE_HPP__
#define __PAN_NET_TCP_PROXY_HANDLER_BASE_HPP__

#include <pan/net/tcp/handler_base.hpp>

namespace pan { namespace net { namespace tcp {

template <typename Inherit, typename Target>
class proxy_handler_base : public pan::net::tcp::handler_base<Inherit> {
    template <typename T1, typename T2> friend class proxy_handler_base;

public:
    typedef std::function<void(const std::string&, uint16_t)> connect_op_type;

    proxy_handler_base() 
        : associated_handler_(nullptr) 
    { }

    void set_connection_op(connect_op_type op)
    {
        upstream_connect_op_ = std::move(op);
    }

    void set_associated_handler(proxy_handler_base<Target, Inherit>* association)
    {
        associated_handler_ = association;
        if (!association->associated_handler_) {
            association->associated_handler_ = this;
        }
    }

    void on_session_start(session_ptr session)
    {
        // FIXME: think about multi-thread access.
        pool_[session->to_string()] = session;
    }

    void on_session_stop(session_ptr session)
    {
        // FIXME: think about multi-thread access.
        pool_.erase(session->to_string());
    }

    std::map<std::string, session_ptr>& pool() 
    { 
        return pool_; 
    }

    virtual void run() { }

protected:
    void connect(const std::string& host, uint16_t port)
    {
        if (upstream_connect_op_) upstream_connect_op_(host, port);
    }

protected:
    connect_op_type upstream_connect_op_;
    proxy_handler_base<Target, Inherit>* associated_handler_;
    std::map<std::string, session_ptr> pool_;

};

}}}

#endif // __PAN_NET_TCP_PROXY_HANDLER_BASE_HPP__