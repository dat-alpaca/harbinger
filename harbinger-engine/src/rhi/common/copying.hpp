#pragma once
#include "core/defines.hpp"

namespace har
{
    struct texture_copy_loc
    {
        u32 x, y, z;
        u32 level;
    };

    struct texture_copy_dimens
    {
        u32 width, height, depth;
    };
}