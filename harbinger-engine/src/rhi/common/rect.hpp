#pragma once
#include "core/defines.hpp"

namespace har
{
    template <typename T>
    struct rect_t
    {
        T x = 0;
        T y = 0;
        T width;
        T height;
    };

    using rect_i32 = rect_t<i32>;
    using rect_f32 = rect_t<f32>;
}