#ifndef __PAN_LOG_HPP__
#define __PAN_LOG_HPP__

#include <pan/base/singleton.hpp>
#include <pan/base/log/logger.hpp>
#include <pan/base/log/logger_console_handler.hpp>
#include <pan/base/log/logger_file_handler.hpp>
#include <pan/base/log/logger_network_handler.hpp>

namespace pan {

class log_wrapper {
public:
    log_wrapper() : logger_(std::make_shared<logger_console_handler>()) { }
    logger& ref() { return logger_; }
    const logger& ref() const { return logger_; }
private:
    logger logger_;
};

#define LOG_INSTANCE                 pan::singleton<log_wrapper>::instance().ref()

#define LOG_CONSOLE                  LOG_INSTANCE.handler(std::make_shared<logger_console_handler>());
#define LOG_FILE                     LOG_INSTANCE.handler(std::make_shared<logger_file_handler>());
#define LOG_NETWORK                  LOG_INSTANCE.handler(std::make_shared<logger_network_handler>());

#define LOG_SET_LEVEL(levek)         LOG_INSTANCE.level(level)
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

#endif // __PAN_LOG_HPP__