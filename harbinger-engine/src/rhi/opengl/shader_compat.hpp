#pragma once
#include <spirv_glsl.hpp>

#include "core/config/config.hpp"
#include "core/logging/logger.hpp"
#include "core/utils/api_utils.hpp"
#include "core/containers/containers.hpp"

namespace har::gl
{
    inline har_string convert_spv_to_gl_glsl(const config_t& config, std::span<u32> data)
	{
		try
		{
			spirv_cross::CompilerGLSL compiler(data.data(), data.size());
			auto options = compiler.get_common_options();
			{
				options.version = get_glsl_version(config);
				options.es = false;
    			compiler.set_common_options(options);
			}
            return har_string { compiler.compile() };
		}
		catch (const std::exception& e)
		{
			HAR_LOG_CRITICAL("Failed to convert SPIRV data to GLSL. {}", e.what());
            throw;
		}
	}
}