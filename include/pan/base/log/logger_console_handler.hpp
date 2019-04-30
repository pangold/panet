#ifndef __PAN_LOGGER_CONSOLE_HANDLER_HPP__
#define __PAN_LOGGER_CONSOLE_HANDLER_HPP__

#include <iostream>
#include <pan/base/log/logger.hpp>

namespace pan {

class logger_console_handler : public logger_handler_base {
public:
    void process(logger_level level, const std::string& info)
    {
        // TODO: different color for different level
        std::cout << info << std::endl;
        // TODO: restore
    }
};

}

#endif // __PAN_LOGGER_CONSOLE_HANDLER_HPP__