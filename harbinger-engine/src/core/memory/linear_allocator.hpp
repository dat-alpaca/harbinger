#pragma once
#include "core/defines.hpp"

namespace har
{
    class linear_allocator
    {
    public:
        linear_allocator(byte* underlyingBuffer, u32 capacity);

        linear_allocator() = default;

    public:
        void initialize(byte* underlyingBuffer, u32 capacity);

    public:
        byte* push(u32 size, u8 alignment = sizeof(u64));

        void clear();

    private:
        NON_OWNING byte* mUnderlyingBuffer = nullptr;
        u32 mCapacity = 0;
        u32 mSize = 0;
    };

    using static_allocator    = linear_allocator;
    using transient_allocator = linear_allocator;
}