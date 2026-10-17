#pragma once
#include "core/defines.hpp"

namespace har
{
    enum class vertex_layout : u8
    {
        none        = 0,
        position    = 1 << 0,
        normals     = 1 << 1,
        tangents    = 1 << 2,
        uv0         = 1 << 3,
    };
    HAR_DEFINE_ENUM(vertex_layout);
}