#pragma once
#include "core/defines.hpp"
#include <cassert>
#include <cstddef>
#include <span>
#include <type_traits>

namespace har
{
    constexpr inline bool is_power_of_two(u64 number)
    {
        return (number & (number - 1)) == 0;   
    }

    constexpr inline u64 align_address(u64 address, u8 alignment)
    {
        if (address == 0)
            return address;

        assert(is_power_of_two(alignment));
        return (address + alignment - 1) & ~(alignment - 1);
    }

    template <typename T, size_t Extent>
    requires(!std::is_volatile_v<T>)
    inline std::span<const har::byte, (Extent == std::dynamic_extent) ? std::dynamic_extent : Extent * sizeof(T)> as_bytes(std::span<T, Extent> span) noexcept
    {
        auto data = reinterpret_cast<const har::byte*>(span.data());
        auto size = span.size_bytes();
        constexpr auto extent = (Extent == std::dynamic_extent) ? std::dynamic_extent : Extent * sizeof(T);
        return std::span<const har::byte, extent> { data, size };
    }

    template <typename T, size_t Extent>
    requires(!std::is_const_v<T> && !std::is_volatile_v<T>)
    inline std::span<har::byte, (Extent == std::dynamic_extent) ? std::dynamic_extent : Extent * sizeof(T)> as_writeable_bytes(std::span<T, Extent> span) noexcept
    {
        auto data = reinterpret_cast<har::byte*>(span.data());
        auto size = span.size_bytes();
        constexpr auto extent = (Extent == std::dynamic_extent) ? std::dynamic_extent : Extent * sizeof(T);
        return std::span<har::byte, extent> { data, size };
    }
}