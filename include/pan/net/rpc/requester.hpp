#ifndef __PAN_NET_RPC_REQUESTER_HPP__
#define __PAN_NET_RPC_REQUESTER_HPP__

#include <pan/base.hpp>
#include <pan/net/protobuf.hpp>
#include <pan/net/rpc/processor.hpp>

namespace pan { namespace net { namespace rpc {

/* 
 * It's a specialization of protobuf::processor<Session, Message>
 * for processing protobuf message Pango::Rpc::Request
 * But, where is this message from? Client. 
 * So, mostly, here's server.
 */
template <typename Session>
class processor<Session, Pango::Rpc::Request> : public processor_base<Session> {
    typedef processor_base<Session> _Mybase;
    typedef stream_executor<istream, ostream> executor_type;
public:
    typedef Pango::Rpc::Request message_type;
    typedef Pango::Rpc::Respond reply_type;
    typedef std::shared_ptr<message_type> message_ptr;

    processor(pool_type& pool, codec_type& codec)
        : _Mybase("Pango.Rpc.Request", pool, codec)
    {
        using namespace std::placeholders;
        auto cb = std::bind(&processor::on_message, this, _1, _2);
        codec.register_callback<message_type>(cb);
    }

    // Args... chould be blank or only with object
    template <typename F, typename... Args>
    void bind(const std::string& name, F func, Args... args)
    {
        executor_.bind(name, func, std::forward<Args>(args)...);
    }

private:
    void on_message(session_ptr session, message_ptr message)
    {
        ostream os;
        istream is(message->params().data(), message->params().size());
        auto reply = std::make_shared<reply_type>();
        reply->set_name(message->name());
        reply->set_id(message->id());
        try {
            executor_.invoke(message->name(), os, is);
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
        codec().send(session, reply);
    }

private:
    executor_type executor_;
};

}}}

#endif // __PAN_NET_RPC_REQUESTER_HPP__