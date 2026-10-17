#pragma once
#include "core/defines.hpp"
#include "renderer/vertex_layout.hpp"

namespace har
{
    struct mesh
    {
        u32 firstVertex                 = 0;
        u32 firstIndex                  = 0;
        u32 indexCount                  = 0;
        vertex_layout vertexLayout      = vertex_layout::none;
    };

    using mesh_handle = small_handle_t;
}