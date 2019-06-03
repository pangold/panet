#ifndef __PAN_NET_TCP_CONNECTOR_HPP__
#define __PAN_NET_TCP_CONNECTOR_HPP__

#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <pan/base.hpp>

namespace pan { namespace net { namespace tcp {

class handler_base;
class connector : public noncopyable {
public:
    typedef handler_base handler_type;

    connector(boost::asio::io_context& io_context, const std::string& host, const std::string& port, handler_type& handler);
    virtual ~connector();

private:
    void resolve_(const std::string& host, const std::string& port);
    void connect_(const boost::asio::ip::tcp::resolver::results_type& endpoints);

private:
    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::resolver resolver_;
    handler_type& handler_;

};

}}}

#include <pan/net/tcp/connector_impl.hpp>

#endif // __PAN_NET_TCP_CONNECTOR_HPP__
