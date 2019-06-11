#ifndef __PAN_NET_TCP_CLIENT_HPP__
#define __PAN_NET_TCP_CLIENT_HPP__

#include <boost/asio.hpp>
#include <pan/base.hpp>
#include <pan/net/tcp/session.hpp>

namespace pan { namespace net { namespace tcp {
    
/*
 * A connection blocking client that only hold one session.
 * Multiple sessions won't be supported.
 * Mainly design as a end-client.
 */
template <typename Handler>
class client : public pan::noncopyable {
    friend typename Handler;
public:
    typedef Handler handler_type;
    typedef connector<handler_type> connector_type;
    typedef session<handler_type> session_type;
    typedef typename session_type::key_type key_type;
    typedef typename session_type::pointer session_ptr;

    explicit client(const std::string& host, uint16_t port)
        : io_context_()
        , handler_()
        , connector_(io_context_, handler_)
    {
        using namespace std::placeholders;
        connector_.register_new_session_callback(std::bind(&client::new_session, this, _1));
        connector_.register_close_session_callback(std::bind(&client::close_session, this, _1));
        connector_.connect(host, port);
        thread_ = std::thread([this]() { io_context_.run(); });
    }

    virtual ~client()
    {
        stop();
        thread_.join();
    }

    void stop()
    {
        if (!session_) return;
        // must manually stop, 
        // because read() of session keeps one ref_counter, 
        // and use_count will never be 0 
        session_->stop(); 
        session_.reset(); 
    }

    void write(const void* data, size_t size)
    {
        wait_for_session();
        session_->write(data, size);
    }

    void write(const std::string& data)
    {
        wait_for_session();
        session_->write(data.data(), data.size());
    }

protected:
    void new_session(session_ptr session)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        // here client only manages one session, 
        // so stop previous session.
        this->stop();
        session_ = session;
        cond_.notify_one();
    }

    void close_session(session_ptr session)
    {
        // Note!!! No session->stop() here, here close_session 
        // is originally invoked by session->stop()
        // then session->stop() again, again, and again... then crash...
        session_.reset();
    }

    void wait_for_session()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (!session_) {
            cond_.wait(lock);
        }
    }

protected:
    boost::asio::io_context io_context_;
    handler_type handler_;
    connector_type connector_;
    session_ptr session_;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;

};

}}}

#endif // __PAN_NET_TCP_CLIENT_HPP__