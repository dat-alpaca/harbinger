#include "rhi/opengl/containers.hpp"
#include "rhi/common/rhi_arguments.hpp"

namespace har
{
    u32 get_rhi_required_static_memory(const rhi_arguments& arguments)
    {
        using namespace rhi;

        u32 shaderMemory       = shader_container::required_memory(arguments.maxShaderCount);
        u32 bufferMemory       = buffer_container::required_memory(arguments.maxBufferCount);
        u32 textureMemory      = texture_container::required_memory(arguments.maxTextureCount);
        u32 samplerMemory      = sampler_container::required_memory(arguments.maxSamplerCount);
        u32 renderTargetMemory = render_target_container::required_memory(arguments.maxRenderTargetCount);
        u32 pipelineMemory     = pipeline_container::required_memory(arguments.maxPipelineCount);
        u32 setMemory          = descriptor_set_container::required_memory(arguments.maxDescriptorSets);
        u32 combinedTexMemory  = descriptor_set_container::required_memory(arguments.maxCombinedTextureCount);

        return
            shaderMemory          + 
            bufferMemory          + 
            textureMemory         + 
            samplerMemory         + 
            renderTargetMemory    + 
            pipelineMemory        +
            setMemory             +
            combinedTexMemory;
    }

    u32 get_rhi_required_dynamic_memory(const rhi_arguments& arguments)
    {
        // TODO: add variables here: vertex_attribute max and descriptor_set max, colorAttachments
        return 0;
    }
}