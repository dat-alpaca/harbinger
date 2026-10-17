#pragma once

#if defined(HAR_GL)
#   include "rhi/opengl/rhi_gl.hpp"
#   include "rhi/opengl/command_buffer_gl.hpp"
    using rhi_t = har::rhi::rhi_gl;
    using command_buffer_t = har::rhi::command_buffer_gl;
#else
    #error Graphics API not supported
#endif
