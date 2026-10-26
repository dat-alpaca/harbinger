#pragma once
#include <memory>

#include "core/assert/assert.hpp"
#include "core/defines.hpp"

namespace har
{
    template<typename T>
    struct fixed_offset_buffer
    {
    public:
        constexpr static u64 required_memory(u32 elementCount)
        {
            return sizeof(T) * elementCount;
        }

    public:
        constexpr void initialize(byte* backingBuffer, u32 elementCount)
        {
            HAR_ASS(backingBuffer);
            HAR_ASS(elementCount);

            mBuffer = reinterpret_cast<T*>(backingBuffer);
            mCapacity = elementCount;
            mFixedIndex = 0;
            mIndex = 0;
        }

        constexpr void insert_fixed(const T& value)
        {
            HAR_ASS(!(mFixedIndex >= mCapacity))
            std::construct_at(&mBuffer[mFixedIndex++], value);
            mIndex = mFixedIndex;
        }

        constexpr void insert(const T& value)
        {
            HAR_ASS(!(mIndex >= mCapacity))
            std::construct_at(&mBuffer[mIndex++], value);
        }

        constexpr void clear_after_offset()
        {
            for (u32 i = mIndex; i > mFixedIndex; --i)
                std::destroy_at(&mBuffer[i - 1]);

            mIndex = mFixedIndex;
        }

        constexpr void clear()
        {
            for (u32 i = mIndex; i > 0; --i)
                std::destroy_at(&mBuffer[i - 1]);

            mIndex = 0;
            mFixedIndex = 0;
        }

    public:
        constexpr T& operator[](handle_t index)
        {
            HAR_ASS(index < mIndex);
            return mBuffer[index];
        }
        constexpr const T& operator[](handle_t index) const
        {
            HAR_ASS(index < mIndex);
            return mBuffer[index];
        }

    public:
        constexpr bool is_empty() const { return mIndex == 0; }
        constexpr u32 capacity() const { return mCapacity; }
        constexpr u32 size() const { return mIndex; }

    private:
        T* mBuffer = nullptr;
        u32 mCapacity = 0;
        u32 mFixedIndex = 0;
        u32 mIndex = 0;
    };
}