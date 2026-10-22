#pragma once
#include "core/assert/assert.hpp"
#include "core/defines.hpp"
#include "core/memory/dynamic_allocator.hpp"
#include "core/memory/linear_allocator.hpp"
#include "core/memory/memory_tag.hpp"
#include "core/memory/allocators.hpp"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>
#include <type_traits>

namespace har
{
    template<typename T, memory_tag tag>
    struct dynamic_allocator_wrapper
    {
        using value_type      = T;
        using reference       = T&;
        using const_reference = const T&;

        using propagate_on_container_copy_assignment = std::true_type;
        using propagate_on_container_move_assignment = std::false_type;
        using propagate_on_container_swap            = std::false_type;

        template<typename U>
        struct rebind
        {
            using other = dynamic_allocator_wrapper<U, tag>;
        };

    public:
        constexpr dynamic_allocator_wrapper() noexcept 
            : rDynamicAllocator(&internal::MainDynamicAllocator) {}

        constexpr dynamic_allocator_wrapper(NON_OWNING dynamic_allocator* allocator) noexcept 
            : rDynamicAllocator(allocator) {}

        template<typename U>
        constexpr dynamic_allocator_wrapper(const dynamic_allocator_wrapper<U, tag>& other) noexcept 
            : rDynamicAllocator(other.get_allocator()) {}

    public:
        constexpr T* allocate(u64 count)
        {
            HAR_ASS(rDynamicAllocator);
            return reinterpret_cast<T*>(rDynamicAllocator->allocate(tag, count * sizeof(T), 1));
        }

        constexpr void deallocate(T* buffer, [[maybe_unused]] u64 count) noexcept
        {
            /* Blank */
        }

    public:
        constexpr bool operator==(const dynamic_allocator_wrapper& other) const noexcept
        {
            return rDynamicAllocator == other.rDynamicAllocator;
        }

        constexpr bool operator!=(const dynamic_allocator_wrapper& other) const noexcept
        {
            return !(*this == other);
        }

    public:
        constexpr dynamic_allocator* get_allocator() const { return rDynamicAllocator; }

    private:
        NON_OWNING dynamic_allocator* rDynamicAllocator = nullptr;
    };

    using har_string = std::basic_string<char, std::char_traits<char>, dynamic_allocator_wrapper<char, memory_tag::string>>;

    template<memory_tag tag, typename T>
    using har_vector = std::vector<T, dynamic_allocator_wrapper<T, tag>>;

    template
    <
        typename Key, 
        typename Value, 
        typename Hash = std::hash<Key>, 
        typename Equal = std::equal_to<Key>
    >
    using har_unordered_map = std::pmr::unordered_map<Key, Value, Hash, Equal>;
}

namespace har
{
    template<typename T, memory_tag tag>
    struct transient_allocator_wrapper
    {
        using value_type      = T;
        using reference       = T&;
        using const_reference = const T&;

        using propagate_on_container_copy_assignment = std::true_type;
        using propagate_on_container_move_assignment = std::false_type;
        using propagate_on_container_swap            = std::false_type;

        template<typename U>
        struct rebind
        {
            using other = transient_allocator_wrapper<U, tag>;
        };

    public:
        constexpr transient_allocator_wrapper() noexcept 
            : rTransientAllocator(&internal::MainTransientAllocator) {}

        constexpr transient_allocator_wrapper(NON_OWNING transient_allocator* allocator) noexcept 
            : rTransientAllocator(allocator) {}

        template<typename U>
        constexpr transient_allocator_wrapper(const transient_allocator_wrapper<U, tag>& other) noexcept 
            : rTransientAllocator(other.get_allocator()) {}

    public:
        constexpr T* allocate(u64 count)
        {
            HAR_ASS(rTransientAllocator);
            return reinterpret_cast<T*>(rTransientAllocator->push(count * sizeof(T), 1));
        }

        constexpr void deallocate(T* buffer, [[maybe_unused]] u64 count) noexcept
        {
            /* Blank */
        }

    public:
        constexpr bool operator==(const transient_allocator_wrapper& other) const noexcept
        {
            return rTransientAllocator == other.rTransientAllocator;
        }

        constexpr bool operator!=(const transient_allocator_wrapper& other) const noexcept
        {
            return !(*this == other);
        }

    public:
        constexpr transient_allocator* get_allocator() const { return rTransientAllocator; }

    private:
        NON_OWNING transient_allocator* rTransientAllocator = nullptr;
    };

    template<memory_tag tag, typename T>
    using har_transient_vector = std::vector<T, transient_allocator_wrapper<T, tag>>;
}

namespace har
{
    template<typename T, memory_tag tag>
    struct static_allocator_wrapper
    {
        using value_type      = T;
        using reference       = T&;
        using const_reference = const T&;

        using propagate_on_container_copy_assignment = std::true_type;
        using propagate_on_container_move_assignment = std::false_type;
        using propagate_on_container_swap            = std::false_type;

        template<typename U>
        struct rebind
        {
            using other = static_allocator_wrapper<U, tag>;
        };

    public:
        constexpr static_allocator_wrapper() noexcept 
            : rStaticAllocator(&internal::MainStaticAllocator) {}

        constexpr static_allocator_wrapper(NON_OWNING static_allocator* allocator) noexcept 
            : rStaticAllocator(allocator) {}

        template<typename U>
        constexpr static_allocator_wrapper(const static_allocator_wrapper<U, tag>& other) noexcept 
            : rStaticAllocator(other.get_allocator()) {}

    public:
        constexpr T* allocate(u64 count)
        {
            HAR_ASS(rStaticAllocator);
            return reinterpret_cast<T*>(rStaticAllocator->push(count * sizeof(T), 1));
        }

        constexpr void deallocate(T* buffer, [[maybe_unused]] u64 count) noexcept
        {
            /* Blank */
        }

    public:
        constexpr bool operator==(const static_allocator_wrapper& other) const noexcept
        {
            return rStaticAllocator == other.rStaticAllocator;
        }

        constexpr bool operator!=(const static_allocator_wrapper& other) const noexcept
        {
            return !(*this == other);
        }

    public:
        constexpr static_allocator* get_allocator() const { return rStaticAllocator; }

    private:
        NON_OWNING static_allocator* rStaticAllocator = nullptr;
    };

    template<memory_tag tag, typename T>
    using har_static_vector = std::vector<T, static_allocator_wrapper<T, tag>>;
}