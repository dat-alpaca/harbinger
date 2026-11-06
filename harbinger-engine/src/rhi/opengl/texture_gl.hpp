#pragma once
#include "glad.h"
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
        format_t textureFormat;
        texture_dimension dimension;
    };

    constexpr inline gl_handle get_gl_dimension(texture_dimension dimension)
    {
        switch(dimension)
        {
            case texture_dimension::texture_2d:
                return GL_TEXTURE_2D; 
            
            case texture_dimension::texture_2d_array:
                return GL_TEXTURE_2D_ARRAY;

            default:
            {
                HAR_LOG_CRITICAL("OpenGL: Invalid texture dimension.");
            } break;
        }

        return 0;
    }
}