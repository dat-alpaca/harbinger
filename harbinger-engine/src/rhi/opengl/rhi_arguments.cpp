#include "rhi/opengl/containers.hpp"
#include "rhi/common/rhi_arguments.hpp"

namespace har
{
    u32 rhi_arguments::get_rhi_required_static_memory()
    {
        using namespace rhi;

        u32 shaderMemory       = shader_container::required_memory(maxShaderCount);
        u32 bufferMemory       = buffer_container::required_memory(maxBufferCount);
        u32 textureMemory      = texture_container::required_memory(maxTextureCount);
        u32 samplerMemory      = sampler_container::required_memory(maxSamplerCount);
        u32 renderTargetMemory = render_target_container::required_memory(maxRenderTargetCount);
        u32 pipelineMemory     = pipeline_container::required_memory(maxPipelineCount);
        u32 setMemory          = descriptor_set_container::required_memory(maxDescriptorSets);
        u32 combinedTexMemory  = descriptor_set_container::required_memory(maxCombinedTextureCount);

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

    u32 rhi_arguments::get_rhi_required_dynamic_memory()
    {
        // TODO: add variables here: vertex_attribute max and descriptor_set max, colorAttachments
        return 0;
    }
}