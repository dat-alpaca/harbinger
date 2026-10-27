#pragma once
#include <expected>

#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ResourceLimits.h>

#include "core/containers/containers.hpp"
#include "core/logging/logger.hpp"
#include "core/memory/memory_tag.hpp"
#include "rhi/common/objects.hpp"

namespace har
{
    struct config_t;

    enum class glsl_compiler_error
    {
        compilation_error,
        linking_error,
        reflection_error
    };
    inline const char* get_compiler_error_string(glsl_compiler_error error)
    {
        switch(error)
        {
            case glsl_compiler_error::compilation_error: return "compilation_error";
            case glsl_compiler_error::linking_error: return "linking_error";
            case glsl_compiler_error::reflection_error: return "reflection_error";
        };

        return "invalid load error";
    }

    struct glsl_compiler
    {   
        using glslc_return = std::expected<har_vector<memory_tag::rhi, u32>, glsl_compiler_error>;

    public:
        inline void initialize()
        {
            glslang::InitializeProcess();
        }

        inline void shutdown()
        {
            glslang::FinalizeProcess();
        }

    public:
        glslc_return compile_shader(shader_type type, const har_string& glslText, const config_t& config);

    private:
        inline EShLanguage get_shader_type(shader_type type)
        {
            switch (type)
            {
                case shader_type::vertex: return EShLangVertex;
                case shader_type::fragment: return EShLangFragment;
                default:
                {
                    HAR_LOG_CRITICAL("[GLSL Compiler] Shader '{}' compilation not supported", static_cast<u32>(type));
                    return EShLanguage::EShLangCount;
                }
            }
        }
    };
}