#pragma once
#include <cstring>
#include <functional>
#include <type_traits>
#include "core/defines.hpp"
#include "core/event/event_type.hpp"

namespace har
{
    constexpr static inline u8 gEventSize       = 32;
    constexpr static inline u8 gEventDataSize   = gEventSize - 2;

    struct event
    {
    public:
        template <typename T>
        event(event_type type, const T& value) 
            : type(type) 
        {
            std::memset(data, 0, gEventDataSize);
            set(value); 
        }

        event(event_type type = event_type::invalid) 
            : type(type) 
        { 
            std::memset(data, 0, gEventDataSize);
        }

    public:
        template <typename T>
        inline void set(const T& value)
        {
            static_assert(sizeof(T) <= gEventDataSize);
            static_assert(std::is_trivially_copyable_v<T>);
            std::memcpy(data, &value, sizeof(T));
        }

        template <typename T>
        inline T get() const
        {
            static_assert(sizeof(T) <= gEventDataSize);
            static_assert(std::is_trivially_copyable_v<T>);
            
            T value;
            std::memcpy(&value, data, sizeof(T));

            return value;
        }

    public:
        byte data[gEventDataSize];
        event_type type = event_type::invalid;
        bool handled = false;
    };

    using event_callback = std::function<void(event&)>;

    static_assert(sizeof(event) == gEventSize);
}