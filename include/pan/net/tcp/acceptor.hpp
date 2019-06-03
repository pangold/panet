#ifndef __PAN_NET_TCP_ACCEPTOR_HPP__
#define __PAN_NET_TCP_ACCEPTOR_HPP__

#include <memory>
#include <cstdint>
#include <boost/asio.hpp>
#include <pan/base.hpp>

namespace pan { namespace net { namespace tcp {

class handler_base;
class acceptor : public noncopyable {
public:
    typedef handler_base handler_type;

    acceptor(boost::asio::io_context& io_context, std::uint16_t port, handler_type& handler);
    virtual ~acceptor();

private:
    void accept_();

private:
    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    handler_type& handler_;

};

}}}

#include <pan/net/tcp/acceptor_impl.hpp>

#endif // __PAN_NET_TCP_ACCEPTOR_HPP__
