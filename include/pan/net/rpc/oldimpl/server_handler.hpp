#ifndef __PAN_NET_RPC_SERVER_HANDLER_HPP__
#define __PAN_NET_RPC_SERVER_HANDLER_HPP__

#include <pan/base.hpp>
#include <pan/net/handler_base.hpp>
#include <pan/net/rpc/protocol.hpp>

namespace pan { namespace net { namespace rpc { 

template <typename Session>
class server_handler : public pan::net::handler_base<Session> {
    friend typename Session;
    using executor_type = stream_executor<bit::istream, bit::ostream>;
public:
    explicit server_handler(std::size_t thread_count = 2, std::size_t max_task_per_thread = 1024)
        : codec_()
        , thread_pool_(thread_count, max_task_per_thread)
        , executor_()
    {
        using namespace std::placeholders;
        codec_.register_callback(std::bind(&server_handler::on_datagram, this, _1, _2));
    }

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

private:
    std::size_t on_message(session_ptr session, const void* data, std::size_t size) override
    {
        return codec_.on_message(session, data, size);
    }

    void on_datagram(session_ptr session, datagram_ptr datagram)
    {
        thread_pool_.dispatch(&server_handler::process, this, session, datagram);
    }

    void process(session_ptr session, datagram_ptr datagram)
    {
        if (datagram->request) {
            bit::istream is(datagram->body.data(), datagram->body_size);
            std::string name;
            is >> name;
            codec_.send(session, invoke(name, datagram->message_id, is));
        }
    }

    datagram_ptr invoke(const std::string& name, std::uint32_t message_id, bit::istream& is)
    {
        bit::ostream os;
        auto reply = make_reply(message_id, RPC_STATUS_SUCCESS);
        try {
            executor_.invoke(name, os, is);
            reply->body.assign(os.data(), os.data() + os.size());
            reply->body_size = os.size();
        }
        catch (const executor_type::handler_not_found_error& e) {
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
    codec<session_type> codec_;
    pan::thread_pool thread_pool_;
    executor_type executor_;

};

}}}

#endif // __PAN_NET_RPC_SERVER_HANDLER_HPP__