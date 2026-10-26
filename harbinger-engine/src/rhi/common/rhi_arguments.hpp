#pragma once
#include "core/defines.hpp"

namespace har
{
    struct rhi_arguments
    {
    public:
        u32 get_rhi_required_static_memory();
        u32 get_rhi_required_dynamic_memory();

    public:
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
}