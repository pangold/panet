#ifndef __PAN_NET_RPC_CLIENT_HANDLER_HPP__
#define __PAN_NET_RPC_CLIENT_HANDLER_HPP__

#include <pan/base.hpp>
#include <pan/net/handler_base.hpp>
#include <pan/net/rpc/protocol.hpp>
#include <pan/net/rpc/respond_pool.hpp>

namespace pan { namespace net { namespace rpc {

template <typename Session>
class client_handler : public pan::net::handler_base<Session> {
    friend typename Session;
    static const unsigned short MaxRequest = 32;

public:
    explicit client_handler(std::size_t timeout = 3000) 
        : codec_()
        , session_(nullptr)
        , timeout_(timeout) // 3s
    {
        using namespace std::placeholders;
        codec_.register_callback(std::bind(&client_handler::on_datagram, this, _1, _2));
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
            auto index = static_cast<std::uint32_t>(sequence.index());
            request(name, index, std::forward<Args>(args)...);
            respond(result, index);
            return true;
        }
        catch (const respond_pool::timeout_error& /*e*/) {
            LOG_ERROR("function %s: timeout", name.c_str());
        }
        catch (const std::bad_alloc& e) {
            LOG_ERROR(e.what());
        }
        catch (const std::exception& e) {
            LOG_ERROR(e.what());
        }
        catch (...) {
            LOG_ERROR("function %s: unknown error", name.c_str());
        }
        return false;
    }

private:
    template <typename... Args>
    void request(const std::string& name, std::uint32_t index, Args... args)
    {
        ostream os;
        os << name;
        ostream_write_n(os, std::forward<Args>(args)...);
        codec_.send(session_, make_request(index, os.data(), os.size()));
    }

    template <typename Result>
    void respond(Result& result, std::uint32_t index)
    {
        auto respond = respond_pool_.wait_for(index, timeout_);
        switch (respond->status_code) {
        case RPC_STATUS_SUCCESS:
            istream(respond->body.data(), respond->body.size()) >> result;
            break;
        case RPC_STATUS_FAILURE:
            throw std::exception("call failure");
            break;
        case RPC_STATUS_NOT_FOUND:
            throw std::exception("call not found");
            break;
        case RPC_STATUS_UNKNOWN_ERROR:
            throw std::exception("unknown error");
            break;
        }
    }
    
private:
    void on_datagram(session_ptr /*session*/, datagram_ptr datagram)
    {
        if (!datagram->request && sequencer_.test(datagram->message_id)) {
            respond_pool_.insert(datagram->message_id, datagram);
        }
    }

    std::size_t on_message(session_ptr session, const void* data, std::size_t size) override
    {
        return codec_.on_message(session, data, size);
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
    codec<session_type> codec_;
    session_ptr session_;
    std::size_t timeout_;
    respond_pool respond_pool_;
    sequence<MaxRequest> sequencer_;

};

}}}


#endif // __PAN_NET_RPC_CLIENT_HANDLER_V1_HPP__