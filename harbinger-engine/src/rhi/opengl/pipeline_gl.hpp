#pragma once
#include "core/defines.hpp"
#include "rhi/opengl/handle.hpp"
#include "rhi/common/objects.hpp"
#include "core/memory/memory_tag.hpp"
#include "core/containers/containers.hpp"

namespace har::rhi
{
    struct vertex_input_gl
    {
        har_vector<memory_tag::rhi, vertex_attribute> attributes;
        u8 stride;
    };

    struct color_blending_gl
    {
        har_vector<memory_tag::rhi, blend_attachment> attachments;
        u8 enableBlending : 1;
    };
    
    struct pipeline_gl
    {
        vertex_input_gl vertexInput;
        color_blending_gl colorBlending;
        gl_handle handle;
        rasterizer_t rasterizer;
        input_assembly inputAssembly;
    };
}