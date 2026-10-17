#pragma once
#include <cstring>
#include <expected>

#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ResourceLimits.h>

#include "core/config/config.hpp"
#include "core/containers/containers.hpp"
#include "core/logging/logger.hpp"
#include "core/memory/memory_tag.hpp"
#include "rhi/common/objects.hpp"

namespace har
{
    enum class glsl_compiler_error
    {
        compilation_error,
        linking_error
    };
    inline const char* get_compiler_error_string(glsl_compiler_error error)
    {
        switch(error)
        {
            case glsl_compiler_error::compilation_error: return "compilation_error";
            case glsl_compiler_error::linking_error: return "linking_error";
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
        glslc_return compile_shader(shader_type type, const har_string& glslText, const config_t& config)
        {
            constexpr u32 SpirV_Version = 100;
            constexpr glslang::EShTargetClientVersion VulkanVersion = glslang::EShTargetVulkan_1_3;
            constexpr glslang::EShTargetLanguageVersion SPVVersion = glslang::EShTargetSpv_1_0;

            EShLanguage glslShader = get_shader_type(type);

            // Shader:
            glslang::TShader shader(glslShader);
            shader.setEnvInput(glslang::EShSourceGlsl, glslShader, glslang::EShClientVulkan, SpirV_Version);
            shader.setEnvClient(glslang::EShClientVulkan, VulkanVersion);
            shader.setEnvTarget(glslang::EShTargetSpv, SPVVersion);

            const char* cText = glslText.c_str();
            shader.setStrings(&cText, glslText.size());

            // Resources:
            // TODO: default resources
            if(!shader.parse(/*GetDefaultResources()*/ nullptr, SpirV_Version, false, EShMsgDefault))
            {
                HAR_LOG_ERROR("[GLSL Compiler] Failed to compile GLSL shader. {}", shader.getInfoLog());
                return std::unexpected { glsl_compiler_error::compilation_error };
            }

            glslang::TProgram program;
            program.addShader(&shader);
            if(!program.link(EShMsgDefault))
            {
                HAR_LOG_ERROR("[GLSL Compiler] Failed to link GLSL program. {}", program.getInfoLog());
                return std::unexpected { glsl_compiler_error::linking_error };
            }

            std::vector<u32> spirvData;
            glslang::GlslangToSpv(*program.getIntermediate(EShLangVertex), spirvData);

            har_vector<memory_tag::rhi, u32> result;
            memcpy(result.data(), spirvData.data(), spirvData.size());

            return result;
        }

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