#ifndef __PAN_NET_TCP_PROXY_HANDLER_BASE_HPP__
#define __PAN_NET_TCP_PROXY_HANDLER_BASE_HPP__

#include <pan/net/tcp/handler_base.hpp>
#include <pan/net/protocol/utils.hpp>

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

    // parse each report(datagram)
    std::size_t on_message(session_ptr session, const void* data, std::size_t size)
    {
        std::shared_ptr<protobuf::datagram> datagram;
        int len = protobuf::data_to_datagram(*datagram, data, size);
        if (len == -1) session->stop();
        if (len > 0) on_datagram(session, datagram);
        return len < 1 ? 0 : len;
    }

    // three different ways to dispatch
    // 1. downstream: do something as below, 
    //    a. store old datagram::id(original id from client, need to set it back when reply)
    //    b. change datagram::id to current time(unique for this process)
    //    c. store session(which client this datagram comes from)
    //    d. store datagram(for extension)
    //    then, use new_id as key, store above info into a map(confirm_queue_).
    // 2. upstream: do something as below, 
    //    a. get info from confirm_queue_ by id(new_id)
    //    b. change this datagram::id to old_id
    //    c. reply throught confirm_queue[id]::session::write(not param 1)
    //    d. remove confirm_queue[id](or extension like store this message)
    // 3. local: process this request.
    virtual void on_datagram(session_ptr, std::shared_ptr<protocol::datagram>) = 0;

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