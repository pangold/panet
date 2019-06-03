#ifndef __PAN_NET_TCP_SESSION_HPP__
#define __PAN_NET_TCP_SESSION_HPP__

#include <memory>
#include <cstdint>
#include <boost/asio.hpp>
#include <pan/base.hpp>

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
