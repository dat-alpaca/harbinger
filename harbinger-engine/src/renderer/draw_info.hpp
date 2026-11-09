#pragma once
#include "renderer/sort_key.hpp"

namespace har
{
    struct draw_info
    {
        sort_key sortKey;
        u32 offset;
    };

    struct draw_group
    {
        u32 instanceCount = 0;
        u32 offset;
        sort_key sortKey;
    };
}