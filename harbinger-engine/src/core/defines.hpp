#pragma once
#include <cstdint>
#include <limits>
#include <memory>
#include <utility>

#define UNUSED (void)

#define OWNING
#define NON_OWNING

#define INTERNAL

#define KNib(x)     (x * 500)  /* Kilo-nibble (500x bytes) */
#define KiNib(x)    (x * 512)  /* Kibi-nibble (512x bytes) */

#define KB(x)  (x * 1000)
#define KiB(x) (x * 1024)
#define TO_KB(x) (x / 1000)
#define TO_KiB(x) (x / 1024)

#define MB(x)  (x * 1000 * 1000)
#define MiB(x) (x * 1024 * 1024)
#define TO_MB(x) (x / (1000 * 1000))
#define TO_MiB(x) (x / (1024 * 1024))

#define HAR_DEFINE_ENUM(EnumType) \
    inline EnumType operator|(EnumType lhs, EnumType rhs) { return static_cast<EnumType>(static_cast<u64>(lhs) | static_cast<u64>(rhs)); }; \
    inline EnumType operator&(EnumType lhs, EnumType rhs) { return static_cast<EnumType>(static_cast<u64>(lhs) & static_cast<u64>(rhs)); }; \
    inline bool operator==(EnumType lhs, u64 rhs) { return static_cast<u64>(lhs) == rhs; }; \
    inline bool operator!=(EnumType lhs, u64 rhs) { return static_cast<u64>(lhs) != rhs; };

namespace har
{
    using u8  = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;
    using byte = u8;

    using i32 = int32_t;
    using i64 = int64_t;

    using f32 = float;
    using f64 = double;

    using micro_handle_t = u8;
    using small_handle_t = u16;
    using handle_t = u32;
    using big_handle_t = u64;
    
    template <typename T>
    constexpr inline T invalid_type_t = std::numeric_limits<T>::max();

    constexpr inline micro_handle_t invalid_micro_handle = invalid_type_t<micro_handle_t>;
    constexpr inline small_handle_t invalid_small_handle = invalid_type_t<small_handle_t>;
    constexpr inline handle_t invalid_handle = invalid_type_t<handle_t>;
    constexpr inline big_handle_t invalid_big_handle = invalid_type_t<big_handle_t>;
}

namespace har
{
    template <typename T>
    using har_unique = std::unique_ptr<T>;

    template <typename T>
    using har_shared = std::shared_ptr<T>;

    template <typename T, typename... Args>
    constexpr inline har_unique<T> make_unique(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    constexpr inline har_shared<T> make_shared(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}

namespace har
{
    enum class data_type : u8
    {
        unsigned_byte,
        signed_byte,
        unsigned_short,
        signed_short,
        unsigned_int,
        signed_int,
        float_t,
    };

    enum class file_type
    {
        any,
        text,
        binary,
    };
}