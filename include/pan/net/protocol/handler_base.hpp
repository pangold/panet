#ifndef __PAN_NET_PROTOCOL_HANDLER_BASE_HPP__
#define __PAN_NET_PROTOCOL_HANDLER_BASE_HPP__

#include <pan/net/protocol/datagram.hpp>
#include <pan/net/protocol/utils.hpp>
#include <pan/net/tcp/handler_base.hpp>

namespace pan { namespace net { namespace protocol {

template <typename Inherit>
class handler_base : public tcp::handler_base<Inherit> {
public:
    typedef std::shared_ptr<protocol::datagram> datagram_ptr;

    // decode
    size_t on_message(session_ptr session, const void* data, size_t size)
    {
        size_t process_size = 0;
        while (true) {
            const char* begin = (const char*)data + process_size;
            int remaining_size = size - process_size;
            // not enough space for a datagram, even though datagram::length
            if (remaining_size < 4) break; 
            auto datagram = std::make_shared<protocol::datagram>();
            int len = protocol::data_to_datagram(*datagram, begin, remaining_size);
            if (len == -1) session->stop();
            if (len > 0) on_datagram(session, datagram);
            else break;
            process_size += datagram->size();
        }
        return process_size;
    }

    // three different ways to dispatch
    // 1. downstream: do something as below, 
    //    a. store old datagram::id(original id from client, need to set it back when reply)
    //    b. change datagram::id to current time(unique for this process)
    //    c. store session(which client this datagram comes from)
    //    d. store datagram(for extension)
    //    e. use datagram::name() to get target ip:port, and write
    //    e. then, use new_id as key, store above info into a map(confirm_queue_).
    // 2. upstream: do something as below, 
    //    a. get info from confirm_queue_ by id(new_id)
    //    b. change this datagram::id to old_id
    //    c. reply throught confirm_queue[id]::session::write(not param 1)
    //    d. remove confirm_queue[id](or extension like store this message)
    // 3. local: process this request.
    virtual void on_datagram(session_ptr, datagram_ptr) = 0;

};

}}}

#endif // __PAN_NET_PROTOCOL_HANDLER_BASE_HPP__