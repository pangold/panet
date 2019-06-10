#ifndef __PAN_NET_RPC_RESPONDER_HPP__
#define __PAN_NET_RPC_RESPONDER_HPP__

#include <pan/base.hpp>
#include <pan/net/protobuf.hpp>
#include <pan/net/rpc/processor.hpp>

namespace pan { namespace net { namespace rpc {

/* 
 * It's a specialization of protobuf::processor<Session, Message>
 * for processing protobuf message Pango::Rpc::Respond
 */
template <typename Session>
class processor<Session, Pango::Rpc::Respond> : public processor_base<Session> {
    typedef processor_base<Session> _Mybase;
    typedef pan::map<std::int32_t, std::shared_ptr<Pango::Rpc::Respond>> respond_pool;
    static const unsigned short MaxRequest = 32;
public:
    typedef Pango::Rpc::Request request_type;
    typedef Pango::Rpc::Respond message_type;
    typedef std::shared_ptr<message_type> message_ptr;

    processor(pool_type& pool, codec_type& codec)
        : _Mybase("Pango.Rpc.Respond", pool, codec)
        , timeout_(3000)
    {
        using namespace std::placeholders;
        auto cb = std::bind(&processor::on_message, this, _1, _2);
        codec.register_callback<message_type>(cb);
    }

    void set_timeout(size_t timeout)
    {
        timeout_ = timeout;
    }

    template <typename R, typename... Args>
    R call(std::string& error, const std::string& name, Args... args)
    {
        try {
            sequence_guard<MaxRequest> sequence(sequencer_);
            auto index = static_cast<std::int32_t>(sequence.index());
            request(name, index, std::forward<Args>(args)...);
            return respond<R>(error, index);
        }
        catch (const respond_pool::timeout_error&) {
            error = "call timeout, name: " + name;
            LOG_ERROR(error.c_str());
        }
        catch (const std::exception& e) {
            error = "call exception, name: " + name + ", details: " + std::string(e.what());
            LOG_ERROR(error.c_str());
        }
        return R();
    }

private:
    template <typename... Args>
    void request(const std::string& name, std::int32_t index, Args... args)
    {
        ostream os;
        ostream_write_n(os, std::forward<Args>(args)...);
        auto request = std::make_shared<request_type>();
        request->set_id(index);
        request->set_name(name);
        request->set_params(os.data(), os.size());
        for (auto& e : pool()) {
            codec().send(e.second, request);
        }
    }

    template <typename R>
    R respond(std::string& error, std::int32_t index)
    {
        R result;
        auto respond = respond_pool_.wait_for(index, timeout_);
        switch (respond->code()) {
        case Pango::Rpc::Respond::SUCCESS:
            istream(respond->value().data(), respond->value().size()) >> result;
            return result;
        case Pango::Rpc::Respond::FAILURE:
        case Pango::Rpc::Respond::NOT_FOUND:
            error = respond->value();
            LOG_ERROR(error.c_str());
            break;
        }
        return R();
    }

private:
    void on_message(session_ptr session, message_ptr message)
    {
        if (sequencer_.test(message->id())) {
            respond_pool_.insert(message->id(), message);
        }
    }

private:
    size_t timeout_;
    respond_pool respond_pool_;
    sequence<MaxRequest> sequencer_;
};

}}}

#endif // __PAN_NET_RPC_RESPONDER_HPP__