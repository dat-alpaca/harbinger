#pragma once
#include <algorithm>
#include <memory>

#include "core/assert/assert.hpp"
#include "core/defines.hpp"

namespace har
{
    template<typename T>
    struct nano_free_list
    {
    public:
        struct node
        {
            T value;
            u32 nextFree;  
        };

        constexpr static u64 required_memory(u32 elementCount)
        {
            return sizeof(node) * elementCount;
        }

    public:
        constexpr void initialize(byte* backingBuffer, u32 elementCount)
        {
            HAR_ASS(backingBuffer);
            HAR_ASS(elementCount);

            mBuffer = reinterpret_cast<node*>(backingBuffer);
            mCapacity = elementCount;
            mSize = 0;
            mFree = 0;

            for (u32 i = 0; i < mCapacity; ++i)
                mBuffer[i].nextFree = i + 1;

            mBuffer[elementCount - 1].nextFree = invalid_handle;
        }

        constexpr handle_t insert(const T& value)
        {
            if (mSize >= mCapacity || mFree == invalid_handle)
                return invalid_small_handle;

            handle_t freeIndex = mFree;
            mFree = mBuffer[freeIndex].nextFree;

            std::construct_at(&mBuffer[freeIndex].value, value);
            ++mSize;

            return freeIndex;
        }

        constexpr handle_t insert(T&& value)
        {
            if (mSize >= mCapacity || mFree == invalid_handle)
                return invalid_small_handle;

            handle_t freeIndex = mFree;
            mFree = mBuffer[freeIndex].nextFree;

            std::construct_at(&mBuffer[freeIndex].value, std::move(value));
            ++mSize;

            return freeIndex;
        }

        constexpr void remove(handle_t index)
        {
            HAR_ASS(index < mCapacity);

            mBuffer[index].nextFree = mFree;
            mFree = index;
            --mSize;
        }

    public:
        constexpr T& operator[](handle_t index)
        {
            HAR_ASS(index >= 0);
            return mBuffer[index].value;
        }

        constexpr const T& operator[](handle_t index) const
        {
            HAR_ASS(index >= 0);
            return mBuffer[index].value;
        }

    public:
        constexpr bool is_empty() const { return mSize == 0; }
        constexpr u32 capacity() const { return mCapacity; }
        constexpr u32 size() const { return mSize; }

    private:
        node* mBuffer = nullptr;
        u32 mCapacity = 0;
        u32 mSize = 0;
        u32 mFree = 0;
    };
}