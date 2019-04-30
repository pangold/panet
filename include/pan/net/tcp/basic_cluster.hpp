#ifndef __PAN_NET_TCP_BASIC_CLUSTER_HPP__
#define __PAN_NET_TCP_BASIC_CLUSTER_HPP__

#include <boost/asio.hpp>
#include <pan/base.hpp>
#include <pan/net/tcp/session.hpp>
#include <pan/net/tcp/server.hpp>
#include <pan/net/tcp/client.hpp>

namespace pan { namespace net { namespace tcp {

template <typename Handler>
class basic_cluster : public noncopyable {
    using handler_type = Handler;

public:
    explicit basic_cluster(uint16_t port = 8889)
        : io_context_()
        , handler_()
        , server_(io_context_, port, handler_)
    {
        
    }

    virtual ~basic_cluster()
    {
        for (auto& client : clients_) {
            client.close();
        }
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    handler_type& handler()
    {
        return handler_;
    }

    const handler_type& handler() const
    {
        return handler_;
    }

    void run()
    {
        thread_ = std::thread([this]() { io_context_.run(); });
    }
    
    void connect(const std::string& host, uint16_t port)
    {
        std::string&& key = make_key(host, port);
        auto client = std::make_shared<client>(io_context_, handler_);
        if (client && client->connect(host, port)) {
            clients_[key] = client;
        }
    }
    
    // shouldn't be here, but inside the handler.
    bool say(const std::string& host, uint16_t port, const void* data, size_t size)
    {
        std::string&& key = make_key(host, port);
        auto it = clients_.find(key);
        if (it != clients_.end()) {
            // TODO: say something(a request, respond, notificaiton, etc.)
        }
    }
    
    std::string make_key(const std::string& host, uint16_t port = 8889)
    {
        return "tcp://" + host + ":" + std::to_string(port);
    }

private:
    boost::asio::io_context io_context_;
    handler_type handler_;
    server server_;
    std::map<std::string, std::shared_ptr<client> > clients_;
    std::thread thread_;

};

}}}


#endif // __PAN_NET_TCP_BASIC_CLUSTER_HPP__