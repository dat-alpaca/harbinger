#pragma once
#include "core/containers/nano_free_list.hpp"
#include "rhi/common/objects.hpp"
#include "rhi/opengl/descriptor_gl.hpp"
#include "rhi/opengl/handle.hpp"
#include "rhi/opengl/pipeline_gl.hpp"
#include "rhi/opengl/render_target_gl.hpp"
#include "rhi/opengl/texture_gl.hpp"

namespace har::rhi
{
    using shader_container            = nano_free_list<gl_handle>;
    using buffer_container            = nano_free_list<gl_handle>;
    using texture_container           = nano_free_list<texture_gl>;
    using combined_texture_container  = nano_free_list<combined_texture_descriptor>;
    using sampler_container           = nano_free_list<gl_handle>;
    using render_target_container     = nano_free_list<render_target_gl>;
    using pipeline_container          = nano_free_list<pipeline_gl>;
    using descriptor_set_container    = nano_free_list<descriptor_gl>;
}