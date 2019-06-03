#ifndef __PAN_NET_PROTOBUF_PROXY_HPP__
#define __PAN_NET_PROTOBUF_PROXY_HPP__

#include <memory>
#include <map>
#include <functional>
#include <google/protobuf/message.h>


namespace pan { namespace net { namespace protobuf {


template <typename Session>
class callback_base {
public:
    using session_ptr = typename Session::pointer;
    virtual ~callback_base() { }
    virtual void on_message(session_ptr, message_ptr) = 0;
};


template <typename T, typename Session>
class callback : public callback_base<Session> {
public:
    using value_type = std::function<void(session_ptr, std::shared_ptr<T>)>;

    explicit callback(value_type cb)
        : callback_(std::move(cb))
    { }

    void on_message(session_ptr session, message_ptr message)
    {
        auto p = std::dynamic_pointer_cast<T>(message);
        callback_(session, p);
    }

private:
    value_type callback_;

};


template <typename Session>
class proxy {
    using session_ptr = typename Session::pointer;
    using callback_ptr = std::shared_ptr<callback_base<Session> >;

public:
    template <typename T>
    void register_callback(std::function<void(session_ptr, std::shared_ptr<T>)> cb)
    {
        auto descriptor = T::default_instance().GetDescriptor();
        callbacks_[descriptor] = std::make_shared<callback<T, Session>>(cb);
    }

    void on_message(session_ptr session, message_ptr message)
    {
        auto it = callbacks_.find(message->GetDescriptor());
        if (it == callbacks_.end()) {
            throw std::exception("unsupported message");
        }
        auto descriptor = message->GetDescriptor();
        callbacks_[descriptor]->on_message(session, message);
    }

private:
    std::map<const google::protobuf::Descriptor*, callback_ptr> callbacks_;

};


}}}

#endif // __PAN_NET_PROTOBUF_PROXY_HPP__