#ifndef __PAN_NET_RPC_CONTROLLER_HPP__
#define __PAN_NET_RPC_CONTROLLER_HPP__

template <typename Session>
class controller {
public:
    controller();
    void on_request(session_ptr session, std::shared_ptr<Pango::Rpc::Request> request);
    void on_respond(session_ptr session, std::shared_ptr<Pango::Rpc::Respond> respond);
    void on_heart_beat(session_ptr session, std::shared_ptr<Pango::Rpc::HeartBeat> hb);
    template <typename Result, typename... Args>
    Result call(const std::string& name, Args... args);
private:
    thread_pool thread_pool_;
    executor_type executor_;
    std::size_t timeout_;
    respond_pool respond_pool_;
};

#endif // __PAN_NET_RPC_CONTROLLER_HPP__