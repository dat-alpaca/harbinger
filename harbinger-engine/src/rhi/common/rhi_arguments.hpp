#pragma once
#include "core/defines.hpp"

namespace har
{
    struct rhi_arguments
    {
        u32 maxShaderCount                  = 2;
        u32 maxBufferCount                  = 1;
        u32 maxTextureCount                 = 2;
        u32 maxCombinedTextureCount         = 2;
        u32 maxSamplerCount                 = 2;
        u32 maxRenderTargetCount            = 1;
        u32 maxDescriptorSets               = 1;
        
        u32 maxPipelineCount                = 1;
        u32 maxVertexAttributesPerPipeline  = 8;
    };

    u32 get_rhi_required_static_memory(const rhi_arguments& arguments);
    u32 get_rhi_required_dynamic_memory(const rhi_arguments& arguments);
}