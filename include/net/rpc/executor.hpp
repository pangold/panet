#ifndef __PAN_NET_RPC_EXECUTOR_HPP__
#define __PAN_NET_RPC_EXECUTOR_HPP__

#include <cstdint>
#include <string>
#include <map>
#include <functional>
#include <net/base/stream.hpp>

namespace pan { namespace net { namespace rpc {

// executor is a stream executor, 
// invoke functions by using a bit input stream

class executor {
public:
    class handler_not_found_error : public std::runtime_error {
    public:
        handler_not_found_error(const std::string& name)
            : std::runtime_error("handler [ " + name + " ] is not found. ")
        { }
    };

public:
    template <typename Function>
    void bind(const std::string& name, Function func)
    {
        using namespace std::placeholders;
        handlers_[name] = std::bind(&executor::invoke_handler<Function>, this, func, _1, _2);
    }

    template <typename Function, typename Object>
    void bind(const std::string& name, Function func, Object* obj)
    {
        using namespace std::placeholders;
        handlers_[name] = std::bind(&executor::invoke_handler<Function, Object>, this, func, obj, _1, _2);
    }

    void invoke(const std::string& name, bit::ostream& output, bit::istream& input)
    {
        auto it = handlers_.find(name);
        if (it == handlers_.end()) {
            throw handler_not_found_error(name);
        }
        it->second(output, input);
    }

private:
    template <typename Function>
    void invoke_handler(Function func, bit::ostream& output, bit::istream& input)
    {
        output << invoke_handler_by_stream(func, input);
    }

    template <typename Function, typename Object>
    void invoke_handler(Function func, Object* obj, bit::ostream& output, bit::istream& input)
    {
        output << invoke_handler_by_stream(func, obj, input);
    }

    template <typename Result, typename... Args>
    auto invoke_handler_by_stream(Result(*func)(Args...), bit::istream& input)
    {
        return invoke_handler_by_stream(std::function<Result(Args...)>(func), input);
    }

    template <typename Result, typename Class, typename Object, typename... Args>
    auto invoke_handler_by_stream(Result(Class::* func)(Args...), Object* obj, bit::istream& input)
    {
        using tuple_type = std::tuple<typename std::decay<Args>::type...>;
        auto tuple = bit::istream_to_tuple<tuple_type>(input);
        auto fn = [=](Args... args)->Result {
            return (obj->*func)(args...);
        };
        return invoke_handler_by_tuple<Result>(fn, tuple);
    }

    template <typename Result, typename... Args>
    auto invoke_handler_by_stream(std::function<Result(Args...)> func, bit::istream& input)
    {
        using tuple_type = std::tuple<typename std::decay<Args>::type...>;
        auto tuple = bit::istream_to_tuple<tuple_type>(input);
        return invoke_handler_by_tuple<Result>(func, tuple);
    }

    template <typename Result, typename Function, typename Tuple>
    typename std::enable_if<std::is_same<Result, void>::value, std::uint8_t>::type
        invoke_handler_by_tuple(Function func, Tuple tuple)
    {
        invoke_handler_by_tuple_impl(func, tuple);
        return 0;
    }

    template <typename Result, typename Function, typename Tuple>
    typename std::enable_if<!std::is_same<Result, void>::value, Result>::type
        invoke_handler_by_tuple(Function func, Tuple tuple)
    {
        return invoke_handler_by_tuple_impl(func, tuple);
    }

    // invoke a function by tuple
    template <typename Function, typename Tuple>
    auto invoke_handler_by_tuple_impl(Function func, Tuple tuple)
    {
        constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
        return invoke_handler_impl(std::forward<Function>(func), std::forward<Tuple>(tuple), std::make_index_sequence<size>{});
    }

    template <typename Function, typename Tuple, std::size_t... Ids>
    auto invoke_handler_impl(Function func, Tuple tuple, std::index_sequence<Ids...>)
    {
        return func(std::get<Ids>(std::forward<Tuple>(tuple))...);
    }

private:
    std::map<std::string, std::function<void(bit::ostream&, bit::istream&)>> handlers_;

};

}}}

#endif // __PAN_NET_RPC_EXECUTOR_HPP__