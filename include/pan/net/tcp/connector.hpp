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
    typedef std::function<void(session_ptr)> new_session_callback_type;
    typedef std::function<void(session_ptr)> close_session_callback_type;

    connector(boost::asio::io_context& ios, handler_type& handler)
        : io_context_(ios)
        , resolver_(ios)
        , handler_(handler)
    {

    }

    virtual ~connector()
    {

    }

    void register_new_session_callback(new_session_callback_type cb)
    {
        new_session_callback_ = std::move(cb);
    }

    void register_close_session_callback(close_session_callback_type cb)
    {
        close_session_callback_ = std::move(cb);
    }

    void connect(const std::string& host, uint16_t port)
    {
        LOG_INFO("connector.connect: tcp://%s:%d", host.c_str(), port);
        auto pred = std::bind(&connector::resolved, this, std::placeholders::_1);
        pan::net::asio::resolve(resolver_, host, std::to_string(port), pred);
    }

private:
    void resolved(const boost::asio::ip::tcp::resolver::results_type& endpoints)
    {
        auto pred = std::bind(&connector::connected, this, std::placeholders::_1);
        pan::net::asio::connect(io_context_, endpoints, pred);
    }

    void connected(boost::asio::ip::tcp::socket socket)
    {
        auto session = std::make_shared<session_type>(std::move(socket), handler_);
        session->register_start_callback(new_session_callback_);
        session->register_close_callback(close_session_callback_);
        session->start();
    }

private:
    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::resolver resolver_;
    handler_type& handler_;
    new_session_callback_type new_session_callback_;
    close_session_callback_type close_session_callback_;

};

}}}

#endif // __PAN_NET_TCP_CONNECTOR_HPP__
