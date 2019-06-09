#ifndef __PAN_NET_TCP_CLIENTS_HPP__
#define __PAN_NET_TCP_CLIENTS_HPP__

#include <boost/asio.hpp>
#include <pan/base.hpp>
#include <pan/net/tcp/session.hpp>

namespace pan { namespace net { namespace tcp {
    
template <typename Handler>
class clients : public pan::noncopyable {
    friend typename Handler;
public:
    typedef Handler handler_type;
    typedef connector<handler_type> connector_type;
    typedef session<handler_type> session_type;
    typedef typename session_type::pointer session_ptr;

    clients()
        : io_context_()
        , handler_()
        , connector_(io_context_, handler_)
    {
        using namespace std::placeholders;
        connector_.register_new_session_callback(std::bind(&clients::new_session, this, _1));
        connector_.register_close_session_callback(std::bind(&clients::close_session, this, _1));
    }

    virtual ~clients()
    {
        if (thread_.joinable()) thread_.join();
    }

    // some situations, it may needs message callback. 
    // most situations, message only be processed in internal handler.
    // this is use to extend.
    template <typename Callback>
    void register_message_callback(Callback cb)
    {
        handler_.register_message_callback(cb);
    }

    void connect(const std::string& host, uint16_t port)
    {
        connector_.connect(host, port);
        if (!thread_.joinable()) {
            thread_ = std::thread([this]() { io_context_.run(); });
        }
    }

    bool write(const std::string& ipp, const void* data, size_t size)
    {
        auto it = pool_.find(ipp);
        if (it != pool_.end()) {
            it->second->write(data, size);
        }
        return it != pool_.end();
    }

    bool write(const std::string& ipp, const std::string& data)
    {
        return write(ipp, data.data(), data.size());
    }

protected:
    void new_session(session_ptr session)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pool_[session->to_string()] = session;
    }

    void close_session(session_ptr session)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pool_.erase(session->to_string());
    }

protected:
    boost::asio::io_context io_context_;
    handler_type handler_;
    connector_type connector_;
    std::map<std::string, session_ptr> pool_;
    std::thread thread_;
    std::mutex mutex_;

};

}}}

#endif // __PAN_NET_TCP_CLIENTS_HPP__