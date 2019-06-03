#ifndef __PAN_NET_TCP_SESSION_HPP__
#define __PAN_NET_TCP_SESSION_HPP__

#include <memory>
#include <cstdint>
#include <boost/asio.hpp>
#include <pan/base.hpp>

/* This tcp will not change, the only variable thing is handler.
 * attach handler to tcp module, 
 * if protobuf, attach protobuf_handler to tcp module. 
 * and attach protobuf messages to protobuf_handler.
 * think about combine the real business logic(concrete protobuf message) to handlers.
 *
 * rpc::requester::on_message(std::shared_ptr<Pango::Rpc::Request> message)
 * rpc::responder::on_message(std::shared_ptr<Pango::Rpc::Respond> message)
 * class rpc_handler : public protobuf_handler {};
 * rpc_handler::register_protobuf_message(rpc::requester::on_message, requester_, _1, _2);
 * rpc_handler::register_protobuf_message(rpc::responder::on_message, responder_, _1, _2);
 *
 * combination like these: 
 * tcp::server<rpc_handler>
 * tcp::client<rpc_handler>
 * then, done.....
 */

namespace pan { namespace net { namespace tcp {

class handler_base;
class session 
    : public std::enable_shared_from_this<session>
    , public noncopyable {
public:
    typedef handler_base handler_type;
    typedef pan::buffer<1024> buffer_type;
    typedef std::size_t key_type;
    typedef std::shared_ptr<session> pointer;

public:
    session(key_type id, boost::asio::ip::tcp::socket socket, handler_type& handler);
    virtual ~session();

    key_type id() const { return id_; }

    void start();
    void stop();
    void write(const void* data, std::size_t size);
    void write(const std::string& data);

private:
    void received_(std::size_t size);
    void read_();
    void write_(const char* buffer, std::size_t size);

private:
    key_type id_;
    boost::asio::ip::tcp::socket socket_;
    buffer_type read_buffer_;
    handler_type& handler_;

};

}}}

#include <pan/net/tcp/session_impl.hpp>

#endif // __PAN_NET_TCP_SESSION_HPP__
