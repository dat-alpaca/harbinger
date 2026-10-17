#pragma once
#include "core/defines.hpp"

namespace har
{
    struct color_t
    {
    public:
        constexpr color_t() = default;
        constexpr color_t(f32 r, f32 g, f32 b, f32 a = 1.0f) 
            : r(r)
            , g(g)
            , b(b)
            , a(a) {}

    public:
        constexpr bool operator==(const color_t& other) const { return r == other.r && g == other.g && b == other.b && a == other.a; } 
        constexpr bool operator!=(const color_t& other) const { return !(*this == other); } 

    public:
        f32 r = 0.0f;
        f32 g = 0.0f;
        f32 b = 0.0f;
        f32 a = 0.0f;
    };
}

namespace har::color
{
    constexpr static inline color_t black    = color_t { 0.0f, 0.0f, 0.0f, 1.0f };
    constexpr static inline color_t white    = color_t { 1.0f, 1.0f, 1.0f, 1.0f };
    constexpr static inline color_t red      = color_t { 1.0f, 0.0f, 0.0f, 1.0f };
    constexpr static inline color_t green    = color_t { 0.0f, 1.0f, 0.0f, 1.0f };
    constexpr static inline color_t blue     = color_t { 0.0f, 0.0f, 1.0f, 1.0f };
    constexpr static inline color_t magenta  = color_t { 1.0f, 0.0f, 1.0f, 1.0f };
}