#pragma once
#include "rhi/common/objects.hpp"
#include "rhi/opengl/handle.hpp"

#include "core/defines.hpp"
#include "core/containers/containers.hpp"

namespace har::rhi
{
    struct render_target_gl
    {
        har_vector<memory_tag::rhi, texture_handle> colorAttachments;
        gl_handle renderTargetHandle = invalid_handle;
        gl_handle renderBufferHandle = invalid_handle;
        texture_handle depthAttachment;
    };
}