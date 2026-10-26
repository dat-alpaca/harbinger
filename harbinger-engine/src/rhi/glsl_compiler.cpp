#include "glsl_compiler.hpp"
#include "core/config/config.hpp"

namespace har
{
    glsl_compiler::glslc_return glsl_compiler::compile_shader(shader_type type, const har_string& glslText, const config_t& config)
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
}