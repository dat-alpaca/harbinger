#pragma once
#include "core/defines.hpp"
#include <cstring>

namespace har
{
    constexpr static inline u64 DefaultStringHashFactor = 131;
    constexpr static inline u64 DefaultStringHashModulo = 1e9 + 9;

    inline u64 hash_string(const char* string, u64 factor = DefaultStringHashFactor, u64 modulo = DefaultStringHashModulo)
    {
        u64 result = 0;
        u64 length = std::strlen(string);
        u64 factorPower = 1;

        for (u64 i = 0; i < length; ++i)
        {
            result = (result + (string[i] - 'a' + 1) * factorPower) % modulo;
            factorPower = (factorPower * factor) % modulo;
        }
        return result;
    }
}