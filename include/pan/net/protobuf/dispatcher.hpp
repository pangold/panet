#ifndef __PAN_NET_PROTOBUF_DISPATCHER_HPP__
#define __PAN_NET_PROTOBUF_DISPATCHER_HPP__

#include <map>
#include <memory>
#include <functional>
#include <pan/net/protobuf/helper.hpp>

namespace pan { namespace net { namespace protobuf {

template <typename Session>
class callback_base {
public:
    typedef Session session_type;
    typedef typename session_type::pointer session_ptr;
    virtual ~callback_base() { }
    virtual void on_message(session_ptr, message_ptr) = 0;
};

template <typename T, typename Session>
class callback : public callback_base<Session> {
public:
    typedef std::function<void(session_ptr, std::shared_ptr<T>)> callback_type;
    explicit callback(callback_type cb)
        : callback_(std::move(cb))
    { }
    void on_message(session_ptr session, message_ptr message)
    {
        auto p = std::dynamic_pointer_cast<T>(message);
        callback_(session, p);
    }
private:
    callback_type callback_;
};

template <typename Session>
class dispatcher {
    typedef Session session_type;
    typedef typename session_type::pointer session_ptr;
    typedef std::shared_ptr<callback_base<session_type>> callback_ptr;
    typedef std::map<const descriptor_type*, callback_ptr> callback_map;
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
    callback_map callbacks_;
};

}}}

#endif // __PAN_NET_PROTOBUF_DISPATCHER_HPP__