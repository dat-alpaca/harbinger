#pragma once
#include <spirv_glsl.hpp>
#include "core/config/config.hpp"
#include "core/containers/containers.hpp"

namespace har::gl
{
    har_string convert_spv_to_gl_glsl(const config_t& config, std::span<u32> data);
}