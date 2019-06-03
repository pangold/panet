#ifndef __PAN_NET_TCP_SERVER_HPP__
#define __PAN_NET_TCP_SERVER_HPP__

#include <boost/asio.hpp>
#include <pan/base.hpp>

namespace pan { namespace net { namespace tcp {

template <typename Handler>
class server : public noncopyable {
public:
    typedef Handler handler_type;

    explicit server(std::uint16_t port = 8888);
    virtual ~server();

    void run();
    
    handler_type& handler() { return handler_; }
    const handler_type& handler() const { return handler_; }

private:
    boost::asio::io_context io_context_;
    handler_type handler_;
    acceptor acceptor_;
    std::thread thread_;
    
};

}}}

#include <pan/net/tcp/server_impl.hpp>

#endif // __PAN_NET_TCP_SERVER_HPP__