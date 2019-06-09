#ifndef __PAN_NET_PROXY_HANDLER_HPP__
#define __PAN_NET_PROXY_HANDLER_HPP__

#include <map>
#include <tuple>
#include <pan/net/protocol/handler_base.hpp>
#include <pan/net/proxy/upstream.hpp>
#include <pan/net/tcp/clients.hpp>

namespace pan { namespace net { namespace proxy {

/* 
 * A downstream handler for end-client.
 */
class handler : public protocol::handler_base<handler> {
    typedef std::string service_name_type; // service name
    typedef std::string connect_info_type; // ip, port
    // typedef std::multimap<service_name_type, connect_info_type> service_list;
    typedef std::map<service_name_type, connect_info_type> service_list;
    typedef std::tuple<int64_t, session_ptr, datagram_ptr> confirm_item_tuple;
    typedef std::map<int64_t, confirm_item_tuple> confirm_queue;
public:
    handler() 
    {
        using namespace std::placeholders;
        auto cb = std::bind(&handler::on_upstream, this, _1, _2);
        upstream_.register_message_callback(cb);
        // init services_ from config;
        // temp
        // services_["service_center"] = "127.0.0.1:9999";
        services_["echo"] = "127.0.0.1:8887";
        // then, connect to upstream servers. 
        for (auto& service : services_) {
            auto pos = service.second.find(":");
            auto ip = service.second.substr(0, pos);
            auto port = service.second.substr(pos + 1, service.second.size());
            upstream_.connect(ip, std::atoi(port.c_str()));
        }
    }

private:
    // As downstream handler callback: do something as below, 
    // a. store old datagram::id(original id from client, need to set it back when reply)
    // b. change datagram::id to current time(unique for this process)
    // c. store session(which client this datagram comes from)
    // d. store datagram(for extension)
    //    use new_id as key, above info store into a confirm_queue(map).
    // e. use datagram::name() to get target(ip:port), and write
    void on_datagram(session_ptr session, datagram_ptr datagram)
    {
        auto old_id = datagram->id();
        // id is microseconds
        datagram->set_id(std::chrono::steady_clock::now().time_since_epoch().count() / 1000);
        confirm_queue_[datagram->id()] = std::make_tuple(old_id, session, datagram);
        // transmit to upstream
        std::string data;
        if (protocol::datagram_to_data(data, *datagram)) {
            upstream_.write(services_[datagram->name()], data);
        }
    }

    // As upstream handler callback: do something as below, 
    // a. get info from confirm_queue_ by id(new_id)
    // b. change this datagram::id to old_id
    // c. reply throught confirm_queue[id]::session::write(not param 1)
    // d. remove confirm_queue[id](or extension like store this message)
    void on_upstream(upstream::session_ptr session, upstream::datagram_ptr datagram)
    {
        // TODO: process messages that reply from service center.

        auto it = confirm_queue_.find(datagram->id());
        if (it != confirm_queue_.end()) {
            // recover old id
            datagram->set_id(std::get<0>(it->second));
            std::string data;
            if (protocol::datagram_to_data(data, *datagram)) {
                std::get<1>(it->second)->write(data.data(), data.size());
            }
        }
    }

private:
    tcp::clients<upstream> upstream_;
    service_list services_;
    confirm_queue confirm_queue_;

};

}}}

#endif // __PAN_NET_PROXY_HANDLER_HPP__