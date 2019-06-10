#ifndef __PAN_NET_RPC_SERVER_HANDLER_V2_HPP__
#define __PAN_NET_RPC_SERVER_HANDLER_V2_HPP__

#include <pan/base.hpp>
#include <pan/net/handler_base.hpp>
#include <pan/net/protobuf.hpp>
#include "rpc.pb.h"

namespace pan { namespace net { namespace rpc { namespace v2 {

template <typename Session>
class server_handler : public pan::net::handler_base<Session> {
    friend typename Session;
    using executor_type = stream_executor<istream, ostream>;

public:
    explicit server_handler(std::size_t thread_count = 2, std::size_t max_task_per_thread = 1024)
        : codec_()
        , thread_pool_(thread_count, max_task_per_thread)
        , executor_()
    {
        using namespace std::placeholders;
        codec_.register_callback<Pango::Rpc::Request>(std::bind(&server_handler::on_request, this, _1, _2));
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

protected:
    std::size_t on_message(session_ptr session, const void* data, std::size_t size) override
    {
        // FIXME: this is edge trigger
        try {
            return codec_.on_message(session, data, size);
        }
        catch (const protobuf::data_error& e) {
            LOG_FATAL(e.what());
            if (session) session->stop();
        }
        catch (std::exception& e) {
            LOG_ERROR(e.what());
        }
        return 0;
    }

    void on_request(session_ptr session, std::shared_ptr<Pango::Rpc::Request> request)
    {
        thread_pool_.dispatch(&server_handler::process, this, session, request);
    }

    void process(session_ptr session, std::shared_ptr<Pango::Rpc::Request> request)
    {
        auto reply = std::make_shared<Pango::Rpc::Respond>();
        istream is(request->params().data(), request->params().size());
        ostream os;
        try {
            executor_.invoke(request->name(), os, is);
            reply->set_value(os.data(), os.size());
            reply->set_code(Pango::Rpc::Respond::SUCCESS);
        }
        catch (const executor_type::handler_not_found_error& e) {
            reply->set_code(Pango::Rpc::Respond::NOT_FOUND);
            reply->set_value(e.what());
            LOG_ERROR(e.what());
        }
        catch (const std::exception& e) {
            reply->set_code(Pango::Rpc::Respond::FAILURE);
            reply->set_value(e.what());
            LOG_ERROR(e.what());
        }
        reply->set_id(request->id());
        reply->set_name(request->name());
        codec_.send(session, reply);
    }

private:
    protobuf::codec<session_type> codec_;
    thread_pool thread_pool_;
    executor_type executor_;

};

}}}}

#endif // __PAN_NET_RPC_SERVER_HANDLER_V2_HPP__
