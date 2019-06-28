#ifndef __PAN_NET_PATTERN_REQUESTER_HPP__
#define __PAN_NET_PATTERN_REQUESTER_HPP__

#include <map>
#include <atomic>
#include <memory>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <pan/net/pattern/pattern.pb.h>
#include <pan/net/protocol.hpp>

namespace pan { namespace net { namespace pattern {

/* Basic implement of Request
 * Or, if your expected content(data) is string, just use it by the way.
 * It's a common logic. Most request implements are common or very similar.
 * I don't repeat writing the code over and over again...
 */
template <typename Session>
class requester {
    // Pango::Request is behind this class, 
    // not supposed to be seen outside.
    typedef Pango::Request message_type;
    typedef std::shared_ptr<message_type> message_ptr;

    typedef Session session_type;
    typedef typename session_type::pointer session_ptr;
    typedef protocol::codec<session_type> codec_type;

public:
    // brief: callback the request data(mainly for server side)
    // @param 1: this requestment requested by 
    // @param 2: received requestment data(from client)
    // @param 3: after handling, respond through it(you can see, it's no 'const') 
    //           or just simply ignore it
    // @return: reply code(could be extra codes)
    typedef std::function<int(session_ptr, const std::string&, std::string&)> request_callback_type;

    // brief: callback the responsed data(mainly for client side)
    // @param 1: the reply code
    // @param 2: the resposed data
    typedef std::function<void(int, const std::string&)> reply_callback_type;

    // brief: constructor
    // @codec: protocol codec
    explicit requester(codec_type& codec) 
        : codec_(codec)
    {
        codec.register_callback<message_type>(std::bind(&requester::on_message, this, 
            std::placeholders::_1, std::placeholders::_2));
    }

    // brief: callback the request data to handle outside(for server)
    // @param cb: a callback function
    void register_request_callback(request_callback_type cb)
    {
        request_callback_ = std::move(cb);
    }

    // brief: send (be wrapped by Pango::Request & protocol::datagram)
    // @param session: the target session
    // @param data: data to be sent out.
    int send(session_ptr session, const std::string& data)
    {
        auto message = std::make_shared<message_type>();
        static std::atomic<int> id = 0;
        message->set_id(++id);
        message->set_data(data);
        codec_.send(session, message);
        return id;
    }

    // brief: simply send, no respond
    // @param session: the target session
    // @param data: data to be posted.
    void post(session_ptr session, const std::string& data)
    {
        send(session, data);
    }

    // brief: request with no respond data and synchronization(wait util respond)
    // @param session: the target session
    // @param data: data to be requested.
    // @return: reply code(could be extra reply code)
    bool push(session_ptr session, const std::string& data)
    {
        std::string respond;
        return request(session, data, respond);
    }

    // brief: request with synchronization(wait util respond)
    // @param session: the target session
    // @param data: data to be requested.
    // @param respond: data be responsed.
    // @return: reply code(could be extra reply code)
    bool request(session_ptr session, const std::string& data, std::string& respond)
    {
        int code = 0;
        auto cb = std::bind(&requester::respond, this, std::placeholders::_1, std::placeholders::_2, code, respond);
        async_request(session, data, cb);
        wait_for_respond();
        return code == static_cast<int>(Pango::ReplyCode::SUCCESS);
    }

    // brief: request asynchronously
    // @param session: the target session
    // @param data: data to be requested.
    // @param cb: callback of respond
    void async_request(session_ptr session, const std::string& data, reply_callback_type cb)
    {
        auto id = send(session, data);
        confirmations_[id] = std::move(cb);
        // TODO: timeout
    }

private:
    // the following 3 functions for synchronization(synchronous request)
    void respond(int& c1, std::string& r1, int& c2, std::string& r2)
    {
        std::swap(c1, c2);
        std::swap(r1, r2);
        wokeup();
    }

    void wait_for_respond()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock);
    }

    void wokeup()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        cond_.notify_all();
    }

private:
    void on_message(session_ptr session, message_ptr message)
    {
        if (message->has_code()) {
            on_reply(session, message);
        } else {
            on_request(session, message);
        }
    }

    void on_reply(session_ptr session, message_ptr message)
    {
        auto it = confirmations_.find(message->id());
        if (it != confirmations_.end()) {
            auto& callback = it->second;
            if (callback) callback(message->code(), message->data());
            confirmations_.erase(it);
        }
    }

    void on_request(session_ptr session, message_ptr message)
    {
        if (request_callback_) {
            auto reply = std::make_shared<message_type>();
            auto respond = reply->mutable_data();
            int code = request_callback_(session, message->data(), *respond);
            reply->set_id(message->id());
            reply->set_code(static_cast<Pango::ReplyCode>(code));
            codec_.send(session, reply);
        }
    }

private:
    codec_type codec_;
    request_callback_type request_callback_;
    std::map<int, reply_callback_type> confirmations_;
    std::mutex mutex_;
    std::condition_variable cond_;

};

}}}

#endif // __PAN_NET_PATTERN_REQUESTER_HPP__