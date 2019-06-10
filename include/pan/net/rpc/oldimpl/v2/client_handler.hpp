#ifndef __PAN_NET_RPC_CLIENT_HANDLER_V2_HPP__
#define __PAN_NET_RPC_CLIENT_HANDLER_V2_HPP__

#include <pan/base.hpp>
#include <pan/net/handler_base.hpp>
#include <pan/net/rpc/v2/respond_pool.hpp>
#include <pan/net/protobuf.hpp>
#include "rpc.pb.h"

namespace pan { namespace net { namespace rpc { namespace v2 {

template <typename Session>
class client_handler : public pan::net::handler_base<Session> {
    friend typename Session;
    static const unsigned short MaxRequest = 32;

public:
    explicit client_handler(std::size_t timeout = 3000)
        : codec_()
        , session_(nullptr)
        , timeout_(timeout)
    {
        using namespace std::placeholders;
        codec_.register_callback<Pango::Rpc::Respond>(std::bind(&client_handler::on_respond, this, _1, _2));
    }

    void set_timeout(std::size_t timeout)
    {
        timeout_ = timeout;
    }

    template <typename Result, typename... Args>
    bool call(Result& result, const std::string& name, Args... args)
    {
        if (!session_) {
            LOG_ERROR("function %s: connection is not ready", name.c_str());
        }
        try {
            sequence_guard<MaxRequest> sequence(sequencer_);
            auto index = static_cast<std::int32_t>(sequence.index());
            request(name, index, std::forward<Args>(args)...);
            respond(result, index);
            return true;
        }
        catch (const respond_pool::timeout_error& /*e*/) {
            LOG_ERROR("function %s: timeout", name.c_str());
        }
        catch (const std::exception& e) {
            LOG_ERROR(e.what());
        }
        return false;
    }

private:
    template <typename... Args>
    void request(const std::string& name, std::int32_t index, Args... args)
    {
        ostream os;
        ostream_write_n(os, std::forward<Args>(args)...);
        auto request = std::make_shared<Pango::Rpc::Request>();
        request->set_id(index);
        request->set_name(name);
        request->set_params(os.data(), os.size());
        codec_.send(session_, request);
    }

    template <typename Result>
    void respond(Result& result, std::int32_t index)
    {
        auto respond = respond_pool_.wait_for(index, timeout_);
        switch (respond->code()) {
        case Pango::Rpc::Respond::SUCCESS:
            istream(respond->value().data(), respond->value().size()) >> result;
            break;
        case Pango::Rpc::Respond::FAILURE:
            throw std::exception(respond->value().c_str());
            break;
        case Pango::Rpc::Respond::NOT_FOUND:
            throw std::exception(respond->value().c_str());
            break;
        }
    }

protected:
    void on_respond(session_ptr /*session*/, std::shared_ptr<Pango::Rpc::Respond> respond)
    {
        if (sequencer_.test(respond->id())) {
            respond_pool_.insert(respond->id(), respond);
        }
    }

    std::size_t on_message(session_ptr session, const void* data, std::size_t size) override
    {
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

    void on_start(session_ptr session) override
    {
        session_ = session;
    }

    void on_stop(session_ptr /*session*/) override
    {
        session_ = nullptr;
    }

private:
    protobuf::codec<session_type> codec_;
    session_ptr session_;
    std::size_t timeout_;
    respond_pool respond_pool_;
    sequence<MaxRequest> sequencer_;

};

}}}}

#endif // __PAN_NET_RPC_CLIENT_HANDLER_V2_HPP__
