#ifndef __PAN_LOG_LOGGER_HPP__
#define __PAN_LOG_LOGGER_HPP__

#include <string>
#include <ctime>
#include <chrono>
#include <iostream>
#include <limits>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <memory>
#include <pan/base/thread.hpp>

namespace pan {

enum logger_level {
    LOGGER_LEVEL_DEBUG = 0,
    LOGGER_LEVEL_INFO  = 1,
    LOGGER_LEVEL_WARN  = 2,
    LOGGER_LEVEL_ERROR = 3,
    LOGGER_LEVEL_FATAL = 4
};

const std::vector<std::string> LOGGER_LEVEL_STRING = {
    "[DEBUG]",
    "[ INFO]",
    "[ WARN]",
    "[ERROR]",
    "[FATAL]"
};


class logger_handler_base {
public:
    virtual ~logger_handler_base() { }
    virtual void process(logger_level, const std::string&) = 0;
};
typedef std::shared_ptr<logger_handler_base> logger_handler_ptr;


class logger {
public:
    explicit logger(logger_handler_ptr handler, logger_level level = LOGGER_LEVEL_DEBUG)
        : thread_(std::numeric_limits<std::size_t>::max())
        , handler_(handler)
        , level_(level) 
    {
        thread_.start();
    } 

    virtual ~logger()
    {
        thread_.stop();
    }

    void handler(logger_handler_ptr handler)
    {
        handler_ = handler;
    }
    
    void level(logger_level level)
    {
        level_ = level;
    }
    
    logger_level level() const 
    {
        return level_;
    }

    void print(logger_level level, const char* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        char text[256] = { 0 };
#if defined(_MSC_VER) || defined(_WIN32)
        _vsnprintf(text, sizeof(text), format, ap);
#else
        vsnprintf(text, sizeof(text), format, ap);
#endif
        va_end(ap);
        thread_.push(&logger::print_impl, this, level, std::string(text));
    }

private:
    void print_impl(logger_level level, std::string text)
    {
        if (level >= level_) {
            auto t = LOGGER_LEVEL_STRING[level] + " [" + current_time_string() + "]: " + text;
            handler_->process(level, t);
        }
    }

    std::string current_time_string()
    {
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        char date[32] = { 0 };
        std::strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return std::string(date);
    }

private:
    thread thread_;
    logger_handler_ptr handler_;
    logger_level level_;

};

}

#endif // __PAN_LOG_LOGGER_HPP__