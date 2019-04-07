#ifndef __PAN_NET_RPC_SERVER_HANDLER_HPP__
#define __PAN_NET_RPC_SERVER_HANDLER_HPP__

#include <net/base/stream.hpp>
#include <net/base/thread_pool.hpp>
#include <net/base/log.hpp>
#include <net/handler_base.hpp>
#include <net/rpc/executor.hpp>
#include <net/rpc/datagram.hpp>
#include <net/rpc/codec.hpp>

namespace pan { namespace net { namespace rpc {

template <typename Session>
class server_handler 
    : public pan::net::handler_base<Session> {
public:
    server_handler(std::size_t thread_count = 2, std::size_t max_task_per_thread = 1024)
        : thread_pool_(thread_count, max_task_per_thread)
        , executor_()
    { }

    virtual ~server_handler()
    {
        thread_pool_.clear();
    }

    template <typename Function>
    void bind(const std::string& name, Function func)
    {
        executor_.bind<Function>(name, func);
    }

    template <typename Function, typename Object>
    void bind(const std::string& name, Function func, Object* obj)
    {
        executor_.bind<Function, Object>(name, func, obj);
    }

protected:
    std::size_t on_message(session_ptr session, const void* data, std::size_t size) override
    {
        std::size_t processed_length = 0;
        bit::istream is((char*)data, size);
        while (true) {
            if (is.size() == is.capacity() || is.remaining_size() < RPC_HEADER_LEN) {
                break;
            }
            auto datagram = make_datagram();
            is >> *datagram;
            if (!confirm(*datagram)) {
                break;
            }
            thread_pool_.dispatch(&server_handler::process, this, session, datagram);
            processed_length += datagram->header_size + datagram->body_size;
        }
        return processed_length;
    }

private:
    void process(session_ptr session, datagram_ptr datagram)
    {
        if (datagram->request) {
            bit::istream is(datagram->body.data(), datagram->body_size);
            bit::ostream os, os2;
            std::string name;
            is >> name;
            os2 << *invoke(name, datagram->message_id, os, is);
            session->write(os2.data(), os2.size());
        }
    }

    datagram_ptr invoke(const std::string& name, std::uint32_t message_id, bit::ostream& os, bit::istream& is)
    {
        auto reply = make_reply(message_id, RPC_STATUS_SUCCESS);
        try {
            executor_.invoke(name, os, is);
            reply->body.assign(os.data(), os.data() + os.size());
            reply->body_size = os.size();
        }
        catch (const executor::handler_not_found_error& e) {
            reply->status_code = RPC_STATUS_NOT_FOUND;
            LOG_ERROR(e.what());
        }
        catch (const std::exception& e) {
            reply->status_code = RPC_STATUS_FAILURE;
            LOG_ERROR(e.what());
        }
        catch (...) {
            reply->status_code = RPC_STATUS_UNKNOWN_ERROR;
            LOG_ERROR("unknown error");
        }
        return reply;
    }

private:
    pan::thread_pool thread_pool_;
    executor executor_;

};

}}}

#endif // __PAN_NET_RPC_SERVER_HANDLER_HPP__