#ifndef __PAN_NET_PROTOCOL_DATAGRAM_HANDLER_BASE_HPP__
#define __PAN_NET_PROTOCOL_DATAGRAM_HANDLER_BASE_HPP__

#include <pan/net/protocol/datagram.hpp>
#include <pan/net/protocol/utils.hpp>
#include <pan/net/tcp/handler_base.hpp>

namespace pan { namespace net { namespace protocol {

template <typename Inherit>
class datagram_handler_base : public tcp::handler_base<Inherit> {
    friend class session_type;
public:
    typedef std::shared_ptr<protocol::datagram> datagram_ptr;

    explicit datagram_handler_base(session_pool_type& pool)
        : tcp::handler_base<Inherit>(pool)
    { }

    virtual ~datagram_handler_base() { }

protected:
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

    // process datagram.
    virtual void on_datagram(session_ptr, datagram_ptr) = 0;

};

}}}

#endif // __PAN_NET_PROTOCOL_DATAGRAM_HANDLER_BASE_HPP__