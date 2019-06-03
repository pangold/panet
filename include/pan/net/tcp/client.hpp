#ifndef __PAN_NET_TCP_CLIENT_HPP__
#define __PAN_NET_TCP_CLIENT_HPP__

#include <boost/asio.hpp>
#include <pan/base.hpp>

namespace pan { namespace net { namespace tcp {
    
template <typename Handler>
class client : public noncopyable {
public:
    typedef Handler handler_type;

    explicit client(const std::string& host = "localhost", uint16_t port = 8888);
    virtual ~client();

    handler_type& handler() { return handler_; }
    const handler_type& handler() const { return handler_; }

private:
    boost::asio::io_context io_context_;
    handler_type handler_;
    connector connector_;
    std::thread thread_;

};

}}}

#include <pan/net/tcp/client_impl.hpp>

#endif // __PAN_NET_TCP_CLIENT_HPP__