#include "shader_compat.hpp"

#include "core/logging/logger.hpp"
#include "core/utils/api_utils.hpp"
#include "rhi/common/objects.hpp"

namespace har::gl
{
    har_string convert_spv_to_gl_glsl(const config_t& config, std::span<u32> data, shader_type type)
	{
		har_string source;
		try
		{
			spirv_cross::CompilerGLSL compiler(data.data(), data.size());
			auto options = compiler.get_common_options();
			{
				options.version = get_glsl_version(config);
				options.es = false;
                options.vulkan_semantics = false;
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