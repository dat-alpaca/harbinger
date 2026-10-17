#include "linear_allocator.hpp"
#include "core/utils/memory_utils.hpp"
#include <cassert>

namespace har
{
    linear_allocator::linear_allocator(byte* underlyingBuffer, u32 capacity)
        : mUnderlyingBuffer(underlyingBuffer)
        , mCapacity(capacity)
    {
    }

    void linear_allocator::initialize(byte* underlyingBuffer, u32 capacity)
    {
        mUnderlyingBuffer = underlyingBuffer;
        mCapacity = capacity;
    }

    byte* linear_allocator::push(u32 size, u8 alignment)
    {
        assert(is_power_of_two(alignment));
        assert(mUnderlyingBuffer);

        u32 alignedOffset = align_address(mSize, alignment);
        u32 newSize = alignedOffset + size;
        
        assert(newSize <= mCapacity);

        mSize = newSize;

        return &mUnderlyingBuffer[alignedOffset];
    }

    void linear_allocator::clear()
    {
        mSize = 0;
    }
}