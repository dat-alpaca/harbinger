#include "renderer.hpp"

#include "core/defines.hpp"
#include "core/assert/assert.hpp"
#include "core/memory/allocators.hpp"

namespace har
{
    using namespace har::internal;

    void harbinger_renderer::initialize(NON_OWNING rhi_t* rhi, const harbinger_renderer_desc& desc)
    {
        HAR_ASS(rhi);

        rRHI = rhi;
        mCommandBuffer = rRHI->create_command_buffer();
        mTransferBuffer = rRHI->create_command_buffer();

        mMaxDrawInfoPerFrame = desc.maxDrawInfoPerFrame;
    }

    void harbinger_renderer::push_draw_info(graphics_pipeline_handle handle, bool isImmutable)
    {
        mDrawInfo.insert_fixed(draw_info 
        {
            { .pipeline = handle }, 
            mCurrentOffset++ 
        });
    }

    void harbinger_renderer::clear_draw_info()
    {
        mCurrentOffset = 0;

        byte* backingBuffer = MainTransientAllocator.push(mDrawInfo.required_memory(mMaxDrawInfoPerFrame), alignof(u8));
        mDrawInfo.initialize(backingBuffer, mMaxDrawInfoPerFrame);
    }
}