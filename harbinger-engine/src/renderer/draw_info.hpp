#pragma once
#include "renderer/sort_key.hpp"

namespace har
{
    struct draw_info
    {
        sort_key sortKey;
        u64 offset;
    };
}