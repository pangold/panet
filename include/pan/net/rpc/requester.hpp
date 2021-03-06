#ifndef __PAN_NET_RPC_REQUESTER_HPP__
#define __PAN_NET_RPC_REQUESTER_HPP__

#include <pan/base.hpp>
#include <pan/net/protocol.hpp>
#include <pan/net/rpc/processor.hpp>

namespace pan { namespace net { namespace rpc {

template <typename Session>
class requester : public processor_base<Session> {
    typedef stream_executor<istream, ostream> executor_type;
public:
    typedef Pango::Rpc::Request message_type;
    typedef Pango::Rpc::Respond reply_type;
    typedef std::shared_ptr<message_type> message_ptr;

    explicit requester(codec_type& codec)
        : processor_base<Session>("Pango.Rpc.Request", codec)
    {
        using namespace std::placeholders;
        auto cb = std::bind(&requester::on_message, this, _1, _2);
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