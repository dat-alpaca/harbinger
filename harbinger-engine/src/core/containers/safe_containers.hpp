#pragma once
#include <mutex>
#include <utility>

#include "core/defines.hpp"
#include "core/memory/memory_tag.hpp"
#include "core/containers/containers.hpp"

namespace har::thread_safe
{
    template<typename T, memory_tag tag>
    struct safe_transient_vector
    {
        static_assert(std::is_trivially_copyable_v<T>);

    public:
        safe_transient_vector(u32 capacity)
            : mCapacity(capacity)
        {
        }

    public:
        void push_back(const T& value)
        {
            std::lock_guard<std::mutex> lock(mMutex);
            HAR_ASS(mBuffer.size() < mCapacity);

            mBuffer.push_back(value);
        }

        void push_back(T&& value)
        {
            std::lock_guard<std::mutex> lock(mMutex);
            HAR_ASS(mBuffer.size() < mCapacity);

            mBuffer.push_back(std::move(value));
        }

        template<typename... Args>
        void emplace_back(Args&&... args)
        {
            std::lock_guard<std::mutex> lock(mMutex);
            HAR_ASS(mBuffer.size() < mCapacity);

            mBuffer.emplace_back(std::forward<Args>(args)...);
        }

        bool pop_back(T& output)
        {
            std::lock_guard<std::mutex> lock(mMutex);
            if (mBuffer.empty())
                return false;
        
            output = mBuffer.back();
            mBuffer.pop_back();

            return true;
        }

        bool pop_front(T& output)
        {
            std::lock_guard<std::mutex> lock(mMutex);
            if (mBuffer.empty())
                return false;
        
            output = mBuffer.front();
            mBuffer.pop_front();

            return true;
        }

        void clear()
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mBuffer.clear();
        }

    public:
        inline u64 size() const
        {
            std::lock_guard<std::mutex> lock(mMutex);
            return mBuffer.size();
        }

        inline u64 capacity() const
        {
            return mCapacity;
        }

        inline bool is_empty() const
        {
            std::lock_guard<std::mutex> lock(mMutex);
            return mBuffer.empty();
        }

    private:
        har_transient_deque<tag, T> mBuffer;
        mutable std::mutex mMutex;
        u32 mCapacity;
    };
}