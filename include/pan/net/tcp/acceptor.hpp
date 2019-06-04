#ifndef __PAN_NET_TCP_ACCEPTOR_HPP__
#define __PAN_NET_TCP_ACCEPTOR_HPP__

#include <memory>
#include <cstdint>
#include <pan/base.hpp>
#include <pan/net/tcp/asio.hpp>
#include <pan/net/tcp/session.hpp>

namespace pan { namespace net { namespace tcp {

template <typename Handler>
class acceptor : public pan::noncopyable {
public:
    typedef Handler handler_type;
    typedef session<handler_type> session_type;
    typedef typename session_type::pointer session_ptr;
    typedef std::function<void(session_ptr)> session_callback_type;

    acceptor(boost::asio::io_context& io_context, std::uint16_t port, handler_type& handler)
        : io_context_(io_context)
        , acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
        , handler_(handler)
    {
        accept();
    }

    virtual ~acceptor()
    {
        io_context_.post([&]() { acceptor_.close(); });
    }

    void register_session_callback(session_callback_type cb)
    {
        session_callback_ = std::move(cb);
    }

private:
    void accept()
    {
        auto pred = std::bind(&acceptor::accepted, this, std::placeholders::_1);
        pan::net::asio::accept(acceptor_, pred);
    }

    void accepted(boost::asio::ip::tcp::socket socket)
    {
        auto session = std::make_shared<session_type>(std::move(socket), handler_);
        session->start();
        if (session_callback_) session_callback_(session);
    }

private:
    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    handler_type& handler_;
    session_callback_type session_callback_;

};

}}}

#endif // __PAN_NET_TCP_ACCEPTOR_HPP__
