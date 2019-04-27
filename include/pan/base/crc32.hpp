#ifndef __PAN_CRC32_HPP__
#define __PAN_CRC32_HPP__

#include <cstdint>
#include <pan/base/singleton.hpp>

namespace pan {

class crc32 {
public:
    crc32() 
    { 
        init(); 
    }
    void init()
    {
        for (int i = 0; i != 256; i++) {
            uint32_t c = static_cast<uint32_t>(i);
            for (int bit = 0; bit < 8; bit++) {
                if (c & 1) c = (c >> 1) ^ (0xEDB88320);
                else c = c >> 1;
            }
            table_[i] = c;
        }
    }
    std::uint32_t make(std::uint32_t crc, const char* src, std::size_t size)
    {
        while (size--)
            crc = (crc >> 8) ^ (table_[(crc ^ *src++) & 0xff]);
        return crc;
    }
private:
    std::uint32_t table_[256];
};

#define CRC32 pan::singleton<crc32>::instance().make

}

#endif // __PAN_CRC32_HPP__