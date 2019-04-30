#ifndef __PAN_LOGGER_FILE_HANDLER_HPP__
#define __PAN_LOGGER_FILE_HANDLER_HPP__

#include <pan/base/log/logger.hpp>

namespace pan {

class logger_file_handler : public logger_handler_base {
public:
    void process(logger_level level, const std::string& info)
    {

    }
};

}

#endif // __PAN_LOGGER_FILE_HANDLER_HPP__