#ifndef __PAN_NET_TCP_CLIENT_HPP__
#define __PAN_NET_TCP_CLIENT_HPP__

#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <pan/base.hpp>
#include <pan/net/tcp/session.hpp>

namespace pan { namespace net { namespace tcp {

class client : public noncopyable {
public:
    using session_ptr = session::pointer;
    using handler_type = session::handler_type;

    client(boost::asio::io_context& io_context, handler_type& handler)
        : io_context_(io_context)
        , handler_(handler)
        , session_(nullptr)
    {

    }

    virtual ~client()
    {
        close();
    }

public:
    bool connect(const std::string& host, std::uint16_t port)
    {
        try {
            connect(host, std::to_string(port));
        } 
        catch (const std::exception& e) {
            LOG_ERROR("tcp.client.connect.error: %s", e.what());
            return false;
        }
        return true;
    }

    void close()
    {
        if (session_) {
            session_->stop();
        }
    }

protected:
    void connect(const std::string& host, const std::string& port)
    {
        boost::asio::ip::tcp::socket socket(io_context_);
        boost::asio::ip::tcp::resolver resolver(io_context_);
        boost::asio::connect(socket, resolver.resolve(host, port));
        session_ = std::make_shared<session>(0, std::move(socket), handler_);
        session_->start();
    }

protected:
    boost::asio::io_context& io_context_;
    handler_type& handler_;
    session_ptr session_;

};

}}}

#endif // __PAN_NET_TCP_CLIENT_HPP__
