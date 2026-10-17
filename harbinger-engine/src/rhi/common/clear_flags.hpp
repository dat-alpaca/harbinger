#pragma once
#include "core/defines.hpp"

namespace har
{
    enum class clear_flags : u8
    {
        none        = 0,
        color       = 1 << 0,
        depth       = 1 << 1,
        stencil     = 1 << 2,
    };
    HAR_DEFINE_ENUM(clear_flags);
}