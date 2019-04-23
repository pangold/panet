#ifndef __PAN_LOGGER_HPP__
#define __PAN_LOGGER_HPP__

#include <string>
#include <ctime>
#include <chrono>
#include <iostream>
#include <limits>
#include <cstdlib>
#include <cstdio>
#include <net/base/singleton.hpp>
#include <net/base/thread.hpp>

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


class logger_handler {
public:
    virtual ~logger_handler() { }
    virtual void process(logger_level, const std::string&) = 0;
};


class console_logger_handler : public logger_handler {
public:
    void process(logger_level level, const std::string& info)
    {
        // TODO: different color for different level
        std::cout << info << std::endl;
        // TODO: restore
    }
};


class file_logger_handler : public logger_handler {
public:
    void process(logger_level level, const std::string& info)
    {

    }
};


class network_logger_handler : public logger_handler {
public:
    void process(logger_level level, const std::string& info)
    {

    }
};


class logger {
public:
    explicit logger(logger_level level = LOGGER_LEVEL_DEBUG)
        : thread_(std::numeric_limits<std::size_t>::max())
        , level_(level) 
        , handler_(std::make_shared<console_logger_handler>())
    {
        thread_.start();
    } 

    virtual ~logger()
    {
        thread_.stop();
    }

    void handler(std::shared_ptr<logger_handler> handler)
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
    logger_level level_;
    std::shared_ptr<logger_handler> handler_;

};

#define LOG_INSTANCE                 singleton<logger>::instance()

#define LOG_CONSOLE                  LOG_INSTANCE.handler(std::make_shared<console_logger_handler>());
#define LOG_FILE                     LOG_INSTANCE.handler(std::make_shared<file_logger_handler>());
#define LOG_NETWORK                  LOG_INSTANCE.handler(std::make_shared<network_logger_handler>());

#define LOG_SET_LEVEL(l)             LOG_INSTANCE.level(l)
#define LOG_SET_LEVEL_DEBUG          LOG_INSTANCE.level(LOGGER_LEVEL_DEBUG)
#define LOG_SET_LEVEL_INFO           LOG_INSTANCE.level(LOGGER_LEVEL_INFO)
#define LOG_SET_LEVEL_WARN           LOG_INSTANCE.level(LOGGER_LEVEL_WARN)
#define LOG_SET_LEVEL_ERROR          LOG_INSTANCE.level(LOGGER_LEVEL_ERROR)
#define LOG_SET_LEVEL_FATAL          LOG_INSTANCE.level(LOGGER_LEVEL_FATAL)
#define LOG_LEVEL                    LOG_INSTANCE.level()

#define LOG_PRINT(level, fmt, ...)   LOG_INSTANCE.print((level), (fmt), __VA_ARGS__)
#define LOG_DEBUG(fmt, ...)          LOG_PRINT(LOGGER_LEVEL_DEBUG, (fmt), __VA_ARGS__)
#define LOG_INFO(fmt, ...)           LOG_PRINT(LOGGER_LEVEL_INFO,  (fmt), __VA_ARGS__)
#define LOG_WARN(fmt, ...)           LOG_PRINT(LOGGER_LEVEL_WARN,  (fmt), __VA_ARGS__)
#define LOG_ERROR(fmt, ...)          LOG_PRINT(LOGGER_LEVEL_ERROR, (fmt), __VA_ARGS__)
#define LOG_FATAL(fmt, ...)          LOG_PRINT(LOGGER_LEVEL_FATAL, (fmt), __VA_ARGS__)

}

#endif // __PAN_LOGGER_HPP__