#pragma once
#include "core/assert/assert.hpp"
#include "dynamic_allocator.hpp"
#include "linear_allocator.hpp"

#include <memory>
#include <utility>

namespace har::internal
{
    inline dynamic_allocator MainDynamicAllocator;
    inline static_allocator MainStaticAllocator;
    inline transient_allocator MainTransientAllocator;
}

// TODO: move
namespace har
{
    template <typename T>
    class static_ptr
    {
    public:
        static_ptr() = default;
        
        static_ptr(const T& other)
        {
            data = internal::MainStaticAllocator.push(sizeof(T), alignof(u8));
            std::construct_at(data, other);
            HAR_ASS(data);
        }
        
        ~static_ptr()
        {
            /* Blank */
        }

    public:
        static_ptr(const static_ptr& other)
            : data(other.data)
        {
        }
        static_ptr(static_ptr&& other) noexcept
            : data(std::exchange(other.data, data))
        {
        }
        static_ptr& operator=(const static_ptr& other)
        {
            static_ptr<T> temp(other);
            std::swap(data, temp.data);
            return *this;
        }
        static_ptr& operator=(static_ptr&& other) noexcept
        {
            std::swap(data, other.data);
            return *this;
        }
    
    public:
        T* operator->()
        {
            return data;
        }

    public:
        T* data = nullptr;
    };

}