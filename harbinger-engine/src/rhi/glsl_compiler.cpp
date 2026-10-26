#include "glsl_compiler.hpp"
#include "core/config/config.hpp"

namespace har
{
    static inline TBuiltInResource get_resources()
    {
        TBuiltInResource defaultResource;

        defaultResource.maxLights                                 = 32;
        defaultResource.maxClipPlanes                             = 6;
        defaultResource.maxTextureUnits                           = 32;
        defaultResource.maxTextureCoords                          = 32;
        defaultResource.maxVertexAttribs                          = 64;
        defaultResource.maxVertexUniformComponents                = 4096;
        defaultResource.maxVaryingFloats                          = 64;
        defaultResource.maxVertexTextureImageUnits                = 32;
        defaultResource.maxCombinedTextureImageUnits              = 80;
        defaultResource.maxTextureImageUnits                      = 32;
        defaultResource.maxFragmentUniformComponents              = 4096;
        defaultResource.maxDrawBuffers                            = 32;
        defaultResource.maxVertexUniformVectors                   = 128;
        defaultResource.maxVaryingVectors                         = 8;
        defaultResource.maxFragmentUniformVectors                 = 16;
        defaultResource.maxVertexOutputVectors                    = 16;
        defaultResource.maxFragmentInputVectors                   = 15;
        defaultResource.minProgramTexelOffset                     = -8;
        defaultResource.maxProgramTexelOffset                     = 7;
        defaultResource.maxClipDistances                          = 8;
        defaultResource.maxComputeWorkGroupCountX                 = 65535;
        defaultResource.maxComputeWorkGroupCountY                 = 65535;
        defaultResource.maxComputeWorkGroupCountZ                 = 65535;
        defaultResource.maxComputeWorkGroupSizeX                  = 1024;
        defaultResource.maxComputeWorkGroupSizeY                  = 1024;
        defaultResource.maxComputeWorkGroupSizeZ                  = 64;
        defaultResource.maxComputeUniformComponents               = 1024;
        defaultResource.maxComputeTextureImageUnits               = 16;
        defaultResource.maxComputeImageUniforms                   = 8;
        defaultResource.maxComputeAtomicCounters                  = 8;
        defaultResource.maxComputeAtomicCounterBuffers            = 1;
        defaultResource.maxVaryingComponents                      = 60;
        defaultResource.maxVertexOutputComponents                 = 64;
        defaultResource.maxGeometryInputComponents                = 64;
        defaultResource.maxGeometryOutputComponents               = 128;
        defaultResource.maxFragmentInputComponents                = 128;
        defaultResource.maxImageUnits                             = 8;
        defaultResource.maxCombinedImageUnitsAndFragmentOutputs   = 8;
        defaultResource.maxCombinedShaderOutputResources          = 8;
        defaultResource.maxImageSamples                           = 0;
        defaultResource.maxVertexImageUniforms                    = 0;
        defaultResource.maxTessControlImageUniforms               = 0;
        defaultResource.maxTessEvaluationImageUniforms            = 0;
        defaultResource.maxGeometryImageUniforms                  = 0;
        defaultResource.maxFragmentImageUniforms                  = 8;
        defaultResource.maxCombinedImageUniforms                  = 8;
        defaultResource.maxGeometryTextureImageUnits              = 16;
        defaultResource.maxGeometryOutputVertices                 = 256;
        defaultResource.maxGeometryTotalOutputComponents          = 1024;
        defaultResource.maxGeometryUniformComponents              = 1024;
        defaultResource.maxGeometryVaryingComponents              = 64;
        defaultResource.maxTessControlInputComponents             = 128;
        defaultResource.maxTessControlOutputComponents            = 128;
        defaultResource.maxTessControlTextureImageUnits           = 16;
        defaultResource.maxTessControlUniformComponents           = 1024;
        defaultResource.maxTessControlTotalOutputComponents       = 4096;
        defaultResource.maxTessEvaluationInputComponents          = 128;
        defaultResource.maxTessEvaluationOutputComponents         = 128;
        defaultResource.maxTessEvaluationTextureImageUnits        = 16;
        defaultResource.maxTessEvaluationUniformComponents        = 1024;
        defaultResource.maxTessPatchComponents                    = 120;
        defaultResource.maxPatchVertices                          = 32;
        defaultResource.maxTessGenLevel                           = 64;
        defaultResource.maxViewports                              = 16;
        defaultResource.maxVertexAtomicCounters                   = 0;
        defaultResource.maxTessControlAtomicCounters              = 0;
        defaultResource.maxTessEvaluationAtomicCounters           = 0;
        defaultResource.maxGeometryAtomicCounters                 = 0;
        defaultResource.maxFragmentAtomicCounters                 = 8;
        defaultResource.maxCombinedAtomicCounters                 = 8;
        defaultResource.maxAtomicCounterBindings                  = 1;
        defaultResource.maxVertexAtomicCounterBuffers             = 0;
        defaultResource.maxTessControlAtomicCounterBuffers        = 0;
        defaultResource.maxTessEvaluationAtomicCounterBuffers     = 0;
        defaultResource.maxGeometryAtomicCounterBuffers           = 0;
        defaultResource.maxFragmentAtomicCounterBuffers           = 1;
        defaultResource.maxCombinedAtomicCounterBuffers           = 1;
        defaultResource.maxAtomicCounterBufferSize                = 16384;
        defaultResource.maxTransformFeedbackBuffers               = 4;
        defaultResource.maxTransformFeedbackInterleavedComponents = 64;
        defaultResource.maxCullDistances                          = 8;
        defaultResource.maxCombinedClipAndCullDistances           = 8;
        defaultResource.maxSamples                                = 4;
        defaultResource.maxMeshOutputVerticesNV                   = 256;
        defaultResource.maxMeshOutputPrimitivesNV                 = 512;
        defaultResource.maxMeshWorkGroupSizeX_NV                  = 32;
        defaultResource.maxMeshWorkGroupSizeY_NV                  = 1;
        defaultResource.maxMeshWorkGroupSizeZ_NV                  = 1;
        defaultResource.maxTaskWorkGroupSizeX_NV                  = 32;
        defaultResource.maxTaskWorkGroupSizeY_NV                  = 1;
        defaultResource.maxTaskWorkGroupSizeZ_NV                  = 1;
        defaultResource.maxMeshViewCountNV                        = 4;

        defaultResource.limits.nonInductiveForLoops                 = 1;
        defaultResource.limits.whileLoops                           = 1;
        defaultResource.limits.doWhileLoops                         = 1;
        defaultResource.limits.generalUniformIndexing               = 1;
        defaultResource.limits.generalAttributeMatrixVectorIndexing = 1;
        defaultResource.limits.generalVaryingIndexing               = 1;
        defaultResource.limits.generalSamplerIndexing               = 1;
        defaultResource.limits.generalVariableIndexing              = 1;
        defaultResource.limits.generalConstantMatrixVectorIndexing  = 1;

        return defaultResource;
    }

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
        shader.setStrings(&cText, 1);

        // Resources:
        auto resource = get_resources();
        if(!shader.parse(&resource, SpirV_Version, false, EShMsgDefault))
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
        glslang::GlslangToSpv(*program.getIntermediate(glslShader), spirvData);

        har_vector<memory_tag::rhi, u32> result;
        result.resize(spirvData.size());
        memcpy(result.data(), spirvData.data(), spirvData.size() * sizeof(u32));

        return result;
    }
}