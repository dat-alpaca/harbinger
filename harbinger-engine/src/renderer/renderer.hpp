#pragma once
#include <glm/glm.hpp>
#include "core/defines.hpp"
#include "core/containers/fixed_offset_buffer.hpp"

#include "rhi/rhi.hpp"
#include "rhi/common/objects.hpp"

#include "renderer/sort_key.hpp"
#include "renderer/draw_info.hpp"

namespace har
{
    struct draw_group
    {
        sort_key sortKey;
        u32 instanceCount = 0;
    };

    struct object_data
    {
        glm::mat4 model;
    };

    struct harbinger_renderer_desc
    {
    public:
        inline u64 get_required_transient_memory()
        {
            return fixed_offset_buffer<draw_info>::required_memory(maxDrawInfoPerFrame);
        }
        inline u64 get_required_static_memory() 
        { 
            return 0;
        }

    public:
        u64 maxDrawInfoPerFrame;
    };

    class harbinger_renderer
    {
    public:
        void initialize(NON_OWNING rhi_t* rhi, const harbinger_renderer_desc& desc);

    public:
        void push_draw_info(graphics_pipeline_handle handle, bool isImmutable);
        void clear_draw_info();

    private:
        NON_OWNING rhi_t* rRHI = nullptr;
        command_buffer_t mCommandBuffer;
        command_buffer_t mTransferBuffer;
    
    private:
        fixed_offset_buffer<draw_info> mDrawInfo;

    private:
        u32 mCurrentOffset = 0;
        u32 mMaxDrawInfoPerFrame = 0;
    };
}