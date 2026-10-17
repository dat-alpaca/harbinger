#pragma once
#include "rhi/common/format.hpp"
#include "rhi/common/objects.hpp"
#include "rhi/opengl/handle.hpp"

namespace har::rhi
{
    struct texture_gl
    {
        union
        {
            struct
            {
                u32 width;
                u32 height;
            };
            struct
            {
                u32 layerWidth;
                u32 layerHeight;
            };
        };

        u32 arraySize = 1;
        gl_handle handle;
        format textureFormat;
        texture_dimension dimension;
    };
}