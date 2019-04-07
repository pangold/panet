#ifndef __PAN_NET_RPC_CLIENT_HANDLER_HPP__
#define __PAN_NET_RPC_CLIENT_HANDLER_HPP__

#include <net/base/stream.hpp>
#include <net/base/log.hpp>
#include <net/handler_base.hpp>
#include <net/rpc/datagram.hpp>
#include <net/rpc/codec.hpp>
#include <net/rpc/sequence.hpp>
#include <net/rpc/respond_pool.hpp>

namespace pan { namespace net { namespace rpc {

template <typename Session>
class client_handler 
    : public pan::net::handler_base<Session> {
    static const unsigned short MaxRequest = 32;

public:
    client_handler(std::size_t timeout = 3000) 
        : session_(nullptr)
        , timeout_(timeout) // 3s
    { }

    void set_timeout(std::size_t timeout)
    {
        timeout_ = timeout;
    }

    template <typename Result, typename... Args>
    bool call(Result& result, const std::string& name, Args... args)
    {
        bool success = false;
        if (!session_) {
            LOG_ERROR("function %s: connection is not ready", name.c_str());
        }
        try {
            success = call_impl(result, name, std::forward<Args>(args)...);
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
        return success;
    }

private:
    template <typename Result, typename... Args>
    bool call_impl(Result& result, const std::string& name, Args... args)
    {
        bit::ostream os, temp;
        temp << name;
        bit::ostream_write_n(temp, std::forward<Args>(args)...);
        sequence_guard<MaxRequest> sequence(sequencer_);
        auto sequence_index = static_cast<std::uint32_t>(sequence.index());
        auto datagram = make_request(sequence_index, temp.data(), temp.size());
        os << *datagram; // serialize
        session_->write(os.data(), os.size());
        auto respond = respond_pool_.wait_for(sequence_index, timeout_);
        return call_respond(result, name, respond);
    }

    template <typename Result>
    bool call_respond(Result& result, const std::string& name, datagram_ptr respond)
    {
        switch (respond->status_code) {
        case RPC_STATUS_SUCCESS:
            bit::istream(respond->body.data(), respond->body.size()) >> result;
            return true;
        case RPC_STATUS_FAILURE:
            LOG_ERROR("function %s: failed to call", name.c_str());
            break;
        case RPC_STATUS_NOT_FOUND:
            LOG_ERROR("function %s: not found", name.c_str());
            break;
        case RPC_STATUS_UNKNOWN_ERROR:
            LOG_ERROR("function %s: unknown error", name.c_str());
            break;
        }
        return false;
    }
    
private:
    std::size_t on_message(session_ptr /*session*/, const void* data, std::size_t size) override
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
            if (!datagram->request && sequencer_.test(datagram->message_id)) {
                respond_pool_.insert(datagram->message_id, datagram);
            }
            processed_length += datagram->header_size + datagram->body_size;
        }
        return processed_length;
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
    session_ptr session_;
    std::size_t timeout_;
    respond_pool respond_pool_;
    sequence<MaxRequest> sequencer_;

};

}}}


#endif // __PAN_NET_RPC_CLIENT_HANDLER_HPP__