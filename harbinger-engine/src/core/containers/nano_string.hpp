#pragma once
#include <cstring>
#include "core/defines.hpp"
#include "core/assert/assert.hpp"

namespace har
{   
    constexpr static inline u64 gNanoStringSize = 32;
    constexpr static inline u64 gNanoStringDataSize = gNanoStringSize - 1;

    struct nano_string
    {
    public:
        constexpr nano_string() noexcept = default;

        nano_string(const char* string)
        {
            HAR_ASS(string);
            u64 stringSize = std::strlen(string);
            
            HAR_ASS(stringSize <= gNanoStringDataSize);
            length = stringSize;
        
            std::memcpy(data, string, stringSize);            
        }

    public:
        inline bool operator==(nano_string& other) const
        {
            if (other.length != length)
                return false;

            return std::memcmp(data, other.data, gNanoStringSize) == 0;
        }

        inline bool operator!=(nano_string& other) const
        {
            return !(*this == other);
        }

        inline char operator[](u8 index) const
        {
            HAR_ASS(index < gNanoStringDataSize);
            return data[index];
        }

    public:
        constexpr u64 size() const noexcept { return length; }
        constexpr bool empty() const noexcept { return data[0] == 0; }
        constexpr const char* c_str() const { return data; }

    public:
        char data[gNanoStringDataSize] = {};
        u8 length = 0;
    };

    static_assert(sizeof(nano_string) == gNanoStringSize);
}