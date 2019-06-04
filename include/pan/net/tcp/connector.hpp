#ifndef __PAN_NET_TCP_CONNECTOR_HPP__
#define __PAN_NET_TCP_CONNECTOR_HPP__

#include <string>
#include <memory>
#include <pan/base.hpp>
#include <pan/net/tcp/asio.hpp>
#include <pan/net/tcp/session.hpp>

namespace pan { namespace net { namespace tcp {

template <typename Handler>
class connector : public pan::noncopyable {
public:
    typedef Handler handler_type;
    typedef session<handler_type> session_type;
    typedef typename session_type::pointer session_ptr;
    typedef std::function<void(session_ptr)> session_callback_type;

    connector(boost::asio::io_context& ios, const std::string& host, const std::string& port, handler_type& handler) 
        : io_context_(ios)
        , resolver_(ios)
        , handler_(handler)
    {
        connect(host, port);
    }

    virtual ~connector()
    {

    }

    void register_session_callback(session_callback_type cb)
    {
        session_callback_ = std::move(cb);
    }

private:
    void connect(const std::string& host, const std::string& port)
    {
        auto pred = std::bind(&connector::resolved, this, std::placeholders::_1);
        pan::net::asio::resolve(resolver_, host, port, pred);
    }

    void resolved(const boost::asio::ip::tcp::resolver::results_type& endpoints)
    {
        auto pred = std::bind(&connector::connected, this, std::placeholders::_1);
        pan::net::asio::connect(io_context_, endpoints, pred);
    }

    void connected(boost::asio::ip::tcp::socket socket)
    {
        auto session = std::make_shared<session_type>(std::move(socket), handler_);
        session->start();
        if (session_callback_) session_callback_(session);
    }

private:
    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::resolver resolver_;
    handler_type& handler_;
    session_callback_type session_callback_;

};

}}}

#endif // __PAN_NET_TCP_CONNECTOR_HPP__
