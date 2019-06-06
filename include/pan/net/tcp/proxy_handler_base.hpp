#ifndef __PAN_NET_TCP_PROXY_HANDLER_BASE_HPP__
#define __PAN_NET_TCP_PROXY_HANDLER_BASE_HPP__

#include <pan/net/tcp/handler_base.hpp>

namespace pan { namespace net { namespace tcp {

template <typename Inherit, 
    typename Target>
class proxy_handler_base : public handler_base<Inherit> {
    template <typename T1, typename T2> 
    friend class proxy_handler_base;
public:
    typedef std::function<void(const std::string&, uint16_t)> connect_op_type;

    // nothing ...
    proxy_handler_base() 
        : associated_handler_(nullptr) 
    { }

    // connector's connect(ip, port)
    void set_connection_op(connect_op_type op)
    {
        upstream_connect_op_ = std::move(op);
    }

    // associate Inherit and Target.s
    void set_associated_handler(proxy_handler_base<Target, Inherit>* association)
    {
        associated_handler_ = association;
        if (!association->associated_handler_) {
            association->associated_handler_ = this;
        }
    }

    // an extension that use to load info (of upstream handler).
    virtual void run() { }

protected:
    void connect(const std::string& host, uint16_t port)
    {
        if (upstream_connect_op_) upstream_connect_op_(host, port);
    }

protected:
    connect_op_type upstream_connect_op_;
    proxy_handler_base<Target, Inherit>* associated_handler_;

};

}}}

#endif // __PAN_NET_TCP_PROXY_HANDLER_BASE_HPP__